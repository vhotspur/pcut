/*
 * Copyright (c) 2012 Vojtech Horky
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "helper.h"
#include <assert.h>
#include <stdlib.h>

#if defined(PCUT_OS_STDC)
#	include <string.h>
#elif defined(PCUT_OS_HELENOS)
#	include <str.h>
#	include <str_error.h>
#	define strerror str_error
#	define strcmp str_cmp
#	define strncmp str_lcmp
#	define sscanf(string, fmt, storage) ((void)0)
#else
#	error No idea which headers to include.
#endif

int pcut_error_count;
#define MAX_COMMAND_LINE_LENGTH 1024
static char command_line_buffer[MAX_COMMAND_LINE_LENGTH];

static pcut_item_t *pcut_find_by_id(pcut_item_t *first, int id) {
	pcut_item_t *it = pcut_get_real(first);
	while (it != NULL) {
		if (it->id == id) {
			return it;
		}
		it = pcut_get_real_next(it);
	}
	return NULL;
}

static int run_test(pcut_item_t *test, int respawn, const char *prog_path) {
	assert(test->kind == PCUT_KIND_TEST);

	if (respawn) {
		snprintf(command_line_buffer, MAX_COMMAND_LINE_LENGTH,
		    "%s -t%d", prog_path, test->id);
		int exit_ok, exit_status;
		int rc = pcut_respawn(command_line_buffer, &exit_ok, &exit_status);
		printf("    Test `%s': ", test->test.name);
		if (rc != 0) {
			printf("failed to respawn: %s\n", strerror(rc));
			return 2;
		}
		if (exit_ok) {
			if (exit_status == 0) {
				printf("OK\n");
				return 0;
			} else {
				printf("FAILED (%d)\n", exit_status);
				return 1;
			}
		} else {
			printf("ABORTED (%d)\n", exit_status);
			return 3;
		}
	}


	const char *error_message = pcut_run_test(test->test.func);

	if (error_message == NULL) {
		return 0;
	} else {
		printf("    FAILED `%s': %s\n", test->test.name, error_message);
		return 1;
	}
}

static int run_suite(pcut_item_t *suite, pcut_item_t **last, const char *prog_path) {
	pcut_item_t *it = pcut_get_real_next(suite);
	if (it->kind == PCUT_KIND_TESTSUITE) {
		goto leave_ok;
	}

	printf("  Suite `%s'\n", suite->suite.name);

	for (; it != NULL; it = pcut_get_real_next(it)) {
		if (it->kind == PCUT_KIND_TESTSUITE) {
			goto leave_ok;
		}
		if (it->kind != PCUT_KIND_TEST) {
			continue;
		}

		int test_failed = run_test(it, 1, prog_path);
		if (test_failed) {
			goto leave_fail;
		}
	}

leave_ok:
	if (last != NULL) {
		*last = it;
	}
	return 0;

leave_fail:
	if (last != NULL) {
		*last = it;
	}
	return 5;
}


int pcut_main(pcut_item_t *last, int argc, char *argv[]) {
	pcut_item_t *items = pcut_fix_list_get_real_head(last);

	int run_only_suite = -1;
	int run_only_test = -1;

	if (argc > 1) {
		if (strncmp(argv[1], "-s", 2) == 0) {
			sscanf(argv[1] + 2, "%d", &run_only_suite);
		} else if (strncmp(argv[1], "-t", 2) == 0) {
			sscanf(argv[1] + 2, "%d", &run_only_test);
		} else if (strcmp(argv[1], "-l") == 0) {
			pcut_print_tests(items);
			return 0;
		}
	}

	if ((run_only_suite >= 0) && (run_only_test >= 0)) {
		printf("Specify either -s or -t!\n");
		return 1;
	}

	if (run_only_suite > 0) {
		pcut_item_t *suite = pcut_find_by_id(items, run_only_suite);
		if (suite == NULL) {
			printf("Suite not found, aborting!\n");
			return 2;
		}
		if (suite->kind != PCUT_KIND_TESTSUITE) {
			printf("Invalid suite id!\n");
			return 3;
		}

		return run_suite(suite, NULL, argv[0]);
	}

	if (run_only_test > 0) {
		pcut_item_t *test = pcut_find_by_id(items, run_only_test);
		if (test == NULL) {
			printf("Test not found, aborting!\n");
			return 2;
		}
		if (test->kind != PCUT_KIND_TEST) {
			printf("Invalid test id!\n");
			return 3;
		}

		return run_test(test, 0, argv[0]);
	}

	/* Otherwise, run the whole thing. */
	pcut_item_t *it = items;
	while (it != NULL) {
		if (it->kind == PCUT_KIND_TESTSUITE) {
			pcut_item_t *tmp;
			int suite_failed = run_suite(it, &tmp, argv[0]);
			if (suite_failed) {
				/* Do something. */
			}
			it = tmp;
		} else {
			it = pcut_get_real_next(it);
		}
	}

	return 0;
}

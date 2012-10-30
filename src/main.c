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
#include <string.h>

int pcut_error_count;

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

static int run_test(pcut_item_t *test) {
	assert(test->kind == PCUT_KIND_TEST);

	printf("    Test `%s': ", test->test.name);

	const char *error_message = pcut_run_test(test->test.func);
	if (error_message == NULL) {
		printf("OK\n");
		return 0;
	} else {
		printf("FAIL\n      %s\n", error_message);
		return 1;
	}
}

static int run_suite(pcut_item_t *suite, pcut_item_t **last) {
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

		int test_failed = run_test(it);
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

		return run_suite(suite, NULL);
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

		return run_test(test);
	}

	/* Otherwise, run the whole thing. */
	pcut_item_t *it = items;
	while (it != NULL) {
		if (it->kind == PCUT_KIND_TESTSUITE) {
			pcut_item_t *tmp;
			int suite_failed = run_suite(it, &tmp);
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

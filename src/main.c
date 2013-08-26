/*
 * Copyright (c) 2012-2013 Vojtech Horky
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

int pcut_run_mode = PCUT_RUN_MODE_FORKING;

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


static int run_suite(pcut_item_t *suite, pcut_item_t **last, const char *prog_path) {
	pcut_item_t *it = pcut_get_real_next(suite);
	if (it->kind == PCUT_KIND_TESTSUITE) {
		goto leave_no_print;
	}

	int is_first_test = 1;
	int total_count = 0;

	for (; it != NULL; it = pcut_get_real_next(it)) {
		if (it->kind == PCUT_KIND_TESTSUITE) {
			goto leave_ok;
		}
		if (it->kind != PCUT_KIND_TEST) {
			continue;
		}

		if (is_first_test) {
			pcut_report_suite_start(suite);
			is_first_test = 0;
		}

		if (pcut_run_mode == PCUT_RUN_MODE_FORKING) {
			pcut_run_test_forking(prog_path, it);
		} else {
			pcut_run_test_single(it);
		}
		total_count++;
	}

	int ret_val = 0;

leave_ok:
	if (total_count > 0) {
		pcut_report_suite_done(suite);
	}

leave_no_print:
	if (last != NULL) {
		*last = it;
	}

	return ret_val;
}

static void set_setup_teardown_callbacks(pcut_item_t *first) {
	pcut_item_t *active_suite = NULL;
	for (pcut_item_t *it = first; it != NULL; it = pcut_get_real_next(it)) {
		if (it->kind == PCUT_KIND_TESTSUITE) {
			active_suite = it;
		} else if (it->kind == PCUT_KIND_SETUP) {
			if (active_suite != NULL) {
				active_suite->suite.setup = it->setup.func;
			}
			it->kind = PCUT_KIND_SKIP;
		} else if (it->kind == PCUT_KIND_TEARDOWN) {
			if (active_suite != NULL) {
				active_suite->suite.teardown = it->setup.func;
			}
			it->kind = PCUT_KIND_SKIP;
		} else {
			/* Not interesting right now. */
		}
	}
}

int pcut_main(pcut_item_t *last, int argc, char *argv[]) {
	pcut_item_t *items = pcut_fix_list_get_real_head(last);

	int run_only_suite = -1;
	int run_only_test = -1;

	if (argc > 1) {
		int i;
		for (i = 1; i < argc; i++) {
			pcut_is_arg_with_number(argv[i], "-s", &run_only_suite);
			pcut_is_arg_with_number(argv[i], "-t", &run_only_test);
			if (pcut_str_equals(argv[i], "-l")) {
				pcut_print_tests(items);
				return 0;
			}
			if (pcut_str_equals(argv[i], "-u")) {
				pcut_run_mode = PCUT_RUN_MODE_SINGLE;
			}
		}
	}

	setvbuf(stdout, NULL, _IONBF, 0);
	set_setup_teardown_callbacks(items);

	PCUT_DEBUG("run_only_suite = %d   run_only_test = %d", run_only_suite, run_only_test);

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

		int rc;
		if (pcut_run_mode == PCUT_RUN_MODE_SINGLE) {
			rc = pcut_run_test_single(test);
		} else {
			rc = pcut_run_test_forked(test);
		}

		exit(rc);
	}

	/* Otherwise, run the whole thing. */
	pcut_report_init(items);

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

	pcut_report_done();

	return 0;
}

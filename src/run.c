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
#include <setjmp.h>
#include <stdlib.h>

#ifndef PCUT_NO_LONG_JUMP
jmp_buf pcut_bad_test_jmp;
#endif
const char *pcut_bad_test_message;

const char *pcut_run_test(pcut_test_func_t function) {
#ifndef PCUT_NO_LONG_JUMP
	int returned_from_test = setjmp(pcut_bad_test_jmp);

	if (!returned_from_test) {
		function();
		return NULL;
	}

	return pcut_bad_test_message;
#else
	function();
	return NULL;
#endif
}

const char *pcut_run_setup_teardown(pcut_setup_func_t function) {
#ifndef PCUT_NO_LONG_JUMP
	int returned_from_test = setjmp(pcut_bad_test_jmp);

	if (!returned_from_test) {
		function();
		return NULL;
	}

	return pcut_bad_test_message;
#else
	function();
	return NULL;
#endif
}


static pcut_item_t default_suite = {
	.kind = PCUT_KIND_TESTSUITE,
	.id = -1,
	.previous = NULL,
	.next = NULL,
	.suite = {
		.name = "Default",
		.setup = NULL,
		.teardown = NULL
	}
};

static pcut_item_t *pcut_find_parent_suite(pcut_item_t *it) {
	while (it != NULL) {
		if (it->kind == PCUT_KIND_TESTSUITE) {
			return it;
		}
		it = it->previous;
	}
	return &default_suite;
}

static int run_test(pcut_item_t *test, int print_result, int report_result) {
	pcut_item_t *suite = pcut_find_parent_suite(test);
	const char *error_message = NULL;
	const char *teardown_error_message = NULL;

	if (report_result) {
		pcut_report_test_start(test);
	}

	/* Run the set-up function if it was set. */
	if (suite->suite.setup != NULL) {
		error_message = pcut_run_setup_teardown(suite->suite.setup);
		if (error_message != NULL) {
			goto run_teardown;
		}
	}

	/*
	 * Run the test, hopefully we would get some meaningful
	 * error message. Worst case scenario is that this task would
	 * be killed. We cannot do much about that, though.
	 */
	error_message = pcut_run_test(test->test.func);

	/* Run the tear-down function no matter of the test outcome. */
run_teardown:
	if (suite->suite.teardown != NULL) {
		teardown_error_message = pcut_run_setup_teardown(suite->suite.teardown);
	}

	int test_failed = (error_message != NULL) || (teardown_error_message != NULL);

	if (report_result) {
		pcut_report_test_done(test, test_failed, error_message, teardown_error_message, NULL);
	}

	if (print_result) {
		int print_delimeter = 1;

		if (error_message != NULL) {
			if (print_delimeter) {
				printf("%c", 0);
				print_delimeter = 0;
			}
			printf("%s\n", error_message);
		}
		if (teardown_error_message != NULL) {
			if (print_delimeter) {
				printf("%c", 0);
			}
			printf("%s\n", teardown_error_message);
		}
	}

	return test_failed;
}

int pcut_run_test_forked(pcut_item_t *test) {
	return run_test(test, 1, 0);
}

int pcut_run_test_single(pcut_item_t *test) {
	return run_test(test, 0, 1);
}


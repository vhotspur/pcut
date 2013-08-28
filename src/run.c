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

#include "internal.h"
#ifndef PCUT_NO_LONG_JUMP
#include <setjmp.h>
#endif

#ifndef PCUT_NO_LONG_JUMP
static jmp_buf start_test_jump;
#endif

static int execute_teardown_on_failure;
static int report_test_result;
static int print_test_error;
static int leave_means_exit;
static pcut_item_t *current_test = NULL;
static pcut_item_t *current_suite = NULL;

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

static void run_setup_teardown(pcut_setup_func_t func) {
	if (func != NULL) {
		func();
	}
}

static void leave_test(int outcome) {
	if (leave_means_exit) {
		exit(outcome);
	}

#ifndef PCUT_NO_LONG_JUMP
	longjmp(start_test_jump, 1);
#endif
}

void pcut_failed_assertion(const char *message) {
	static const char *prev_message = NULL;
	/*
	 * The assertion failed. We need to abort the current test,
	 * inform the user and perform some clean-up. That could
	 * include running the tear-down routine.
	 */
	if (print_test_error) {
		pcut_print_fail_message(message);
	}

	if (execute_teardown_on_failure) {
		execute_teardown_on_failure = 0;
		prev_message = message;
		run_setup_teardown(current_suite->suite.teardown);

		/* Tear-down was okay. */
		if (report_test_result) {
			pcut_report_test_done(current_test, TEST_OUTCOME_FAIL,
				message, NULL, NULL);
		}
	} else {
		if (report_test_result) {
			pcut_report_test_done(current_test, TEST_OUTCOME_FAIL,
				prev_message, message, NULL);
		}
	}

	prev_message = NULL;

	leave_test(TEST_OUTCOME_FAIL); /* No return. */
}


static int run_test(pcut_item_t *test) {
	/*
	 * Set here as the returning point in case of test failure.
	 * If we get here, it means something failed during the
	 * test execution.
	 */
#ifndef PCUT_NO_LONG_JUMP
	int test_finished = setjmp(start_test_jump);
	if (test_finished) {
		return 1;
	}
#endif

	if (report_test_result) {
		pcut_report_test_start(test);
	}

	current_suite = pcut_find_parent_suite(test);
	current_test = test;

	/*
	 * If anything goes wrong, execute the tear-down function
	 * as well.
	 */
	execute_teardown_on_failure = 1;

	/*
	 * Run the set-up function.
	 */
	run_setup_teardown(current_suite->suite.setup);

	/*
	 * The setup function was performed, it is time to run
	 * the actual test.
	 */
	test->test.func();

	/*
	 * Finally, run the tear-down function. We need to clear
	 * the flag to prevent endless loop.
	 */
	execute_teardown_on_failure = 0;
	run_setup_teardown(current_suite->suite.teardown);

	/*
	 * If we got here, it means everything went well with
	 * this test.
	 */
	if (report_test_result) {
		pcut_report_test_done(current_test, TEST_OUTCOME_PASS,
			NULL, NULL, NULL);
	}

	return 0;
}

int pcut_run_test_forked(pcut_item_t *test) {
	report_test_result = 0;
	print_test_error = 1;
	leave_means_exit = 1;

	int rc = run_test(test);

	current_test = NULL;
	current_suite = NULL;

	return rc;
}

int pcut_run_test_single(pcut_item_t *test) {
	report_test_result = 1;
	print_test_error = 0;
	leave_means_exit = 0;

	int rc = run_test(test);

	current_test = NULL;
	current_suite = NULL;

	return rc;
}


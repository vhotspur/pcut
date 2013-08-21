/*
 * Copyright (c) 2013 Vojtech Horky
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

static int tests_in_suite;
static int failed_tests_in_suite;

void pcut_report_init(pcut_item_t *all_items) {
	printf("Will run %d tests.\n", pcut_count_tests(all_items));
}

void pcut_report_suite_start(pcut_item_t *suite) {
	printf("  Running suite `%s'...\n", suite->suite.name);
	tests_in_suite = 0;
	failed_tests_in_suite = 0;
}

void pcut_report_suite_done(pcut_item_t *suite) {
	printf("  Suite `%s' finished. Tests total: %d. Failed: %d.\n",
				suite->suite.name, tests_in_suite, failed_tests_in_suite);
}

void pcut_report_test_start(pcut_item_t *test) {
	tests_in_suite++;
}

void pcut_report_test_done(pcut_item_t *test, int outcome,
		const char *error_message, const char *teardown_error_message,
		const char *extra_output) {
	const char *test_name = test->test.name;

	if (outcome != 0) {
		failed_tests_in_suite++;
	}

	if (outcome != 0) {
		printf("Failure in %s: %s.\n", test_name, error_message);
	}

	if ((extra_output != NULL) && (extra_output[0] != 0)) {
		printf("Extra output:\n---[ %s ]---\n%s\n---[ %s ]--- (eof)\n",
			test_name, extra_output, test_name);
	}

}

void pcut_report_done() {
}

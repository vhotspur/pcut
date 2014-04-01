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

/** @file
 *
 * Reporting routines for XML output (non-standard).
 */

#include "../internal.h"
#include "report.h"
#ifndef __helenos__
#include <string.h>
#endif
#include <stdio.h>

/** Counter of all run tests. */
static int test_counter;

/** Counter for tests in a current suite. */
static int tests_in_suite;

/** Counter of failed tests in current suite. */
static int failed_tests_in_suite;

/** Initialize the XML output. */
static void xml_init(pcut_item_t *all_items) {
	printf("<?xml version=\"1.0\"?>\n");

	int tests_total = pcut_count_tests(all_items);
	test_counter = 0;

	printf("<report tests-total=\"%d\">\n", tests_total);
}

/** Report that a suite was started. */
static void xml_suite_start(pcut_item_t *suite) {
	tests_in_suite = 0;
	failed_tests_in_suite = 0;

	printf("\t<suite name=\"%s\">\n", suite->suite.name);
}

/** Report that a suite was completed. */
static void xml_suite_done(pcut_item_t *suite) {
	printf("\t</suite><!-- %s: %d / %d -->\n", suite->suite.name,
		failed_tests_in_suite, tests_in_suite);
}

/** Report that a test was started.
 *
 * We do nothing - all handling is done after the test completes.
 *
 * @param test Test that is started.
 */
static void xml_test_start(pcut_item_t *test) {
	PCUT_UNUSED(test);

	tests_in_suite++;
	test_counter++;
}

/** Print the buffer as a CDATA into given element.
 *
 * @param message Message to print.
 * @param element_name Wrapping XML element name.
 */
static void print_by_lines(const char *message, const char *element_name) {
	if ((message == NULL) || (message[0] == 0)) {
		return;
	}

	printf("\t\t\t<%s><![CDATA[", element_name);

	char *next_line_start = pcut_str_find_char(message, '\n');
	while (next_line_start != NULL) {
		next_line_start[0] = 0;
		printf("%s\n", message);
		message = next_line_start + 1;
		next_line_start = pcut_str_find_char(message, '\n');
	}
	if (message[0] != 0) {
		printf("%s\n", message);
	}

	printf("]]></%s>\n", element_name);
}

/** Report a completed test. */
static void xml_test_done(pcut_item_t *test, int outcome,
		const char *error_message, const char *teardown_error_message,
		const char *extra_output) {
	const char *test_name = test->test.name;

	if (outcome != TEST_OUTCOME_PASS) {
		failed_tests_in_suite++;
	}

	const char *status_str = NULL;
	switch (outcome) {
	case TEST_OUTCOME_PASS:
		status_str = "pass";
		break;
	case TEST_OUTCOME_FAIL:
		status_str = "fail";
		break;
	case TEST_OUTCOME_ERROR:
		status_str = "error";
		break;
	default:
		/* Shall not get here. */
		break;
	}

	printf("\t\t<testcase name=\"%s\" status=\"%s\">\n", test_name,
		status_str);

	print_by_lines(error_message, "error-message");
	print_by_lines(teardown_error_message, "error-message");

	print_by_lines(extra_output, "standard-output");

	printf("\t\t</testcase><!-- %s -->\n", test_name);
}

/** Report testing done. */
static void xml_done() {
	printf("</report>\n");
}


pcut_report_ops_t pcut_report_xml = {
	.init = xml_init,
	.done = xml_done,
	.suite_start = xml_suite_start,
	.suite_done = xml_suite_done,
	.test_start = xml_test_start,
	.test_done = xml_test_done
};
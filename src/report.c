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

#include "internal.h"
#ifndef __helenos__
#include <string.h>
#endif

static int test_counter;
static int tests_in_suite;
static int failed_tests_in_suite;

void pcut_print_fail_message(const char *msg) {
	if (msg == NULL) {
		return;
	}
	if (pcut_str_size(msg) == 0) {
		return;
	}

	printf("%c%c%c%s\n%c", 0, 0, 0, msg, 0);
}

#define BUFFER_SIZE 4096
static char buffer_for_extra_output[BUFFER_SIZE];
static char buffer_for_error_messages[BUFFER_SIZE];

static void parse_command_output(const char *full_output, size_t full_output_size,
		char *stdio_buffer, size_t stdio_buffer_size,
		char *error_buffer, size_t error_buffer_size) {
	memset(stdio_buffer, 0, stdio_buffer_size);
	memset(error_buffer, 0, error_buffer_size);

	/* Ensure that we do not read past the full_output. */
	if (full_output[full_output_size - 1] != 0) {
		// FIXME: can this happen?
		return;
	}

	while (1) {
		/* First of all, count number of zero bytes before the text. */
		size_t cont_zeros_count = 0;
		while (full_output[0] == 0) {
			cont_zeros_count++;
			full_output++;
			full_output_size--;
			if (full_output_size == 0) {
				return;
			}
		}

		/* Determine the length of the text after the zeros. */
		size_t message_length = pcut_str_size(full_output);

		if (cont_zeros_count < 2) {
			/* Okay, standard I/O. */
			if (message_length > stdio_buffer_size) {
				// TODO: handle gracefully
				return;
			}
			memcpy(stdio_buffer, full_output, message_length);
			stdio_buffer += message_length;
			stdio_buffer_size -= message_length;
		} else {
			/* Error message. */
			if (message_length > error_buffer_size) {
				// TODO: handle gracefully
				return;
			}
			memcpy(error_buffer, full_output, message_length);
			error_buffer += message_length;
			error_buffer_size -= message_length;
		}

		full_output += message_length + 1;
		full_output_size -= message_length + 1;
	}
}

void pcut_report_init(pcut_item_t *all_items) {
	int tests_total = pcut_count_tests(all_items);
	test_counter = 0;

	printf("1..%d\n", tests_total);
}

void pcut_report_suite_start(pcut_item_t *suite) {
	tests_in_suite = 0;
	failed_tests_in_suite = 0;

	printf("#> Starting suite %s.\n", suite->suite.name);
}

void pcut_report_suite_done(pcut_item_t *suite) {
	printf("#> Finished suite %s (failed %d of %d).\n",
			suite->suite.name, failed_tests_in_suite, tests_in_suite);
}

void pcut_report_test_start(pcut_item_t *test) {
	PCUT_UNUSED(test);

	tests_in_suite++;
	test_counter++;
}

static void print_by_lines(const char *message, const char *prefix) {
	if ((message == NULL) || (message[0] == 0)) {
		return;
	}
	char *next_line_start = pcut_str_find_char(message, '\n');
	while (next_line_start != NULL) {
		next_line_start[0] = 0;
		printf("%s%s\n", prefix, message);
		message = next_line_start + 1;
		next_line_start = pcut_str_find_char(message, '\n');
	}
	if (message[0] != 0) {
		printf("%s%s\n", prefix, message);
	}
}

void pcut_report_test_done(pcut_item_t *test, int outcome,
		const char *error_message, const char *teardown_error_message,
		const char *extra_output) {
	const char *test_name = test->test.name;

	if (outcome != TEST_OUTCOME_PASS) {
		failed_tests_in_suite++;
	}

	const char *status_str = NULL;
	const char *fail_error_str = NULL;
	switch (outcome) {
	case TEST_OUTCOME_PASS:
		status_str = "ok";
		fail_error_str = "";
		break;
	case TEST_OUTCOME_FAIL:
		status_str = "not ok";
		fail_error_str = " failed";
		break;
	case TEST_OUTCOME_ERROR:
		status_str = "not ok";
		fail_error_str = " aborted";
		break;
	default:
		/* Shall not get here. */
		break;
	}
	printf("%s %d %s%s\n", status_str, test_counter, test_name, fail_error_str);

	print_by_lines(error_message, "# error: ");
	print_by_lines(teardown_error_message, "# error: ");

	print_by_lines(extra_output, "# stdio: ");
}

void pcut_report_test_done_unparsed(pcut_item_t *test, int outcome,
		const char *unparsed_output, size_t unparsed_output_size) {

	parse_command_output(unparsed_output, unparsed_output_size,
			buffer_for_extra_output, BUFFER_SIZE,
			buffer_for_error_messages, BUFFER_SIZE);

	pcut_report_test_done(test, outcome, buffer_for_error_messages, NULL, buffer_for_extra_output);
}

void pcut_report_done() {
}


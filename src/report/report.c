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

#include "../internal.h"
#ifndef __helenos__
#include <string.h>
#endif
#include <stdio.h>

static pcut_report_ops_t *report_ops = NULL;

#define REPORT_CALL(op, ...) \
	if ((report_ops != NULL) && (report_ops->op != NULL)) report_ops->op(__VA_ARGS__)

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

void pcut_report_register_handler(pcut_report_ops_t *ops) {
	report_ops = ops;
}

void pcut_report_init(pcut_item_t *all_items) {
	REPORT_CALL(init, all_items);
}

void pcut_report_suite_start(pcut_item_t *suite) {
	REPORT_CALL(suite_start, suite);
}

void pcut_report_suite_done(pcut_item_t *suite) {
	REPORT_CALL(suite_done, suite);
}

void pcut_report_test_start(pcut_item_t *test) {
	REPORT_CALL(test_start, test);
}

void pcut_report_test_done(pcut_item_t *test, int outcome,
		const char *error_message, const char *teardown_error_message,
		const char *extra_output) {
	REPORT_CALL(test_done, test, outcome, error_message, teardown_error_message,
			extra_output);
}

void pcut_report_test_done_unparsed(pcut_item_t *test, int outcome,
		const char *unparsed_output, size_t unparsed_output_size) {

	parse_command_output(unparsed_output, unparsed_output_size,
			buffer_for_extra_output, BUFFER_SIZE,
			buffer_for_error_messages, BUFFER_SIZE);

	pcut_report_test_done(test, outcome, buffer_for_error_messages, NULL, buffer_for_extra_output);
}

void pcut_report_done() {
	REPORT_CALL(done);
}


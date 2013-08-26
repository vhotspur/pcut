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

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../helper.h"

int pcut_run_mode = PCUT_RUN_MODE_FORKING;

#define MAX_TEST_NUMBER_WIDTH 24
#define MAX_COMMAND_LINE_LENGTH 1024
#define OUTPUT_BUFFER_SIZE 8192

static char error_message_buffer[OUTPUT_BUFFER_SIZE];
static char extra_output_buffer[OUTPUT_BUFFER_SIZE];

/* Expects that the whole output is in extra_output_buffer. */
static void parse_extra_output_buffer(void) {
	/* Try to find the error message in the buffer ('\0' delimeted). */
	int extra_output_size = pcut_str_size(extra_output_buffer);

	if (extra_output_size + 1 < OUTPUT_BUFFER_SIZE) {
		const char *error_message_start = extra_output_buffer + extra_output_size + 1;
		if (error_message_start[0] != 0) {
			int error_message_size = OUTPUT_BUFFER_SIZE - 1 - extra_output_size - 1;
			memcpy(error_message_buffer, error_message_start, error_message_size);
		}
	}
}

static void before_test_start(pcut_item_t *test) {
	pcut_report_test_start(test);

	memset(error_message_buffer, 0, OUTPUT_BUFFER_SIZE);
	memset(extra_output_buffer, 0, OUTPUT_BUFFER_SIZE);
}

static size_t read_all(int fd, char *buffer, size_t buffer_size) {
	ssize_t actually_read;
	char *buffer_start = buffer;
	do {
		actually_read = read(fd, buffer, buffer_size);
		if (actually_read > 0) {
			buffer += actually_read;
			buffer_size -= actually_read;
			if (buffer_size == 0) {
				break;
			}
		}
	} while (actually_read > 0);
	if (buffer_start != buffer) {
		if (*(buffer - 1) == 10) {
			*(buffer - 1) = 0;
			buffer--;
		}
	}
	return buffer - buffer_start;
}

static int convert_wait_status_to_outcome(int status) {
	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			return TEST_OUTCOME_FAIL;
		} else {
			return TEST_OUTCOME_PASS;
		}
	}

	if (WIFSIGNALED(status)) {
		return TEST_OUTCOME_ERROR;
	}

	return status;
}

void pcut_run_test_forking(const char *self_path, pcut_item_t *test) {
	PCUT_UNUSED(self_path);

	before_test_start(test);

	int link_stdout[2], link_stderr[2];
	pid_t pid;

	int rc = pipe(link_stdout);
	if (rc == -1) {
		snprintf(error_message_buffer, OUTPUT_BUFFER_SIZE - 1,
				"pipe() failed: %s.", strerror(rc));
		pcut_report_test_done(test, TEST_OUTCOME_ERROR, error_message_buffer, NULL, NULL);
		return;
	}
	rc = pipe(link_stderr);
	if (rc == -1) {
		snprintf(error_message_buffer, OUTPUT_BUFFER_SIZE - 1,
				"pipe() failed: %s.", strerror(rc));
		pcut_report_test_done(test, TEST_OUTCOME_ERROR, error_message_buffer, NULL, NULL);
		return;
	}

	pid = fork();
	if (pid == (pid_t)-1) {
		snprintf(error_message_buffer, OUTPUT_BUFFER_SIZE - 1,
			"fork() failed: %s.", strerror(rc));
		rc = TEST_OUTCOME_ERROR;
		goto leave_close_pipes;
	}

	if (pid == 0) {
		/* We are the child. */
		dup2(link_stdout[1], STDOUT_FILENO);
		close(link_stdout[0]);
		dup2(link_stderr[1], STDERR_FILENO);
		close(link_stderr[0]);

		rc = pcut_run_test_forked(test);

		exit(rc);
	}

	close(link_stdout[1]);
	close(link_stderr[1]);

	size_t stderr_size = read_all(link_stderr[0], extra_output_buffer, OUTPUT_BUFFER_SIZE - 1);
	read_all(link_stdout[0], extra_output_buffer, OUTPUT_BUFFER_SIZE - 1 - stderr_size);

	parse_extra_output_buffer();

	int status;
	wait(&status);

	rc = convert_wait_status_to_outcome(status);

	goto leave_close_parent_pipe;

leave_close_pipes:
	close(link_stdout[1]);
	close(link_stderr[1]);
leave_close_parent_pipe:
	close(link_stdout[0]);
	close(link_stderr[0]);

	pcut_report_test_done(test, rc, error_message_buffer, NULL, extra_output_buffer);
}

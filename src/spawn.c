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
#include <errno.h>
#include <assert.h>
#include "helper.h"

#include <sys/wait.h>

#define MAX_TEST_NUMBER_WIDTH 24
#define MAX_COMMAND_LINE_LENGTH 1024
#define OUTPUT_BUFFER_SIZE 8192

static char command_line_buffer[MAX_COMMAND_LINE_LENGTH];

int pcut_respawn(const char *app_path, const char *argument,
		int *normal_exit, int *exit_code) {

	snprintf(command_line_buffer, MAX_COMMAND_LINE_LENGTH, "%s %s", app_path,
		argument);

	int status = system(command_line_buffer);

	if (status == -1) {
		return errno;
	}

	if (WIFEXITED(status)) {
		/* Normal termination (though a test might failed). */
		*normal_exit = 1;
		*exit_code = WEXITSTATUS(status);
		return 0;
	}

	if (WIFSIGNALED(status)) {
		/* Process was killed. */
		*normal_exit = 0;
		*exit_code = WTERMSIG(status);
		return 0;
	}

	/* We shall not get here. */
	assert(0 && "unreachable code");

	return ENOSYS;
}


static char error_message_buffer[OUTPUT_BUFFER_SIZE];
static char extra_output_buffer[OUTPUT_BUFFER_SIZE];

int pcut_run_test_safe(const char *self_path, pcut_item_t *test,
		char **error_message, char **extra_output) {
	assert(test->kind == PCUT_KIND_TEST);

	/* Clean the buffer first, enable access from outside. */
	strcpy(error_message_buffer, "");
	strcpy(extra_output_buffer, "");
	*error_message = error_message_buffer;
	*extra_output = extra_output_buffer;

	char test_arg[MAX_TEST_NUMBER_WIDTH + 1];
	snprintf(test_arg, MAX_TEST_NUMBER_WIDTH, "-t%d", test->id);
	int exit_ok, exit_status;
	int rc = pcut_respawn(self_path, test_arg, &exit_ok, &exit_status);
	if (rc != 0) {
		snprintf(error_message_buffer, OUTPUT_BUFFER_SIZE - 1,
				"unable to respawn (%s)", strerror(rc));
		return 2;
	}
	if (exit_ok) {
		if (exit_status == 0) {
			return 0;
		} else {
			return 1;
		}
	} else {
		snprintf(error_message_buffer, OUTPUT_BUFFER_SIZE - 1,
				"task was killed (reason: %d)", exit_status);
		return 3;
	}
}

void pcut_run_test_safe_clean(char *error_message, char *extra_output) {
	/* Do nothing. */
	(void) error_message;
	(void) extra_output;
}

/*
 * Add `or' condition for other OSes that do support the `system()'
 * function.
 */

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
#include <string.h>
#include "../helper.h"

#define MAX_TEST_NUMBER_WIDTH 24
#define PCUT_COMMAND_LINE_BUFFER_SIZE 256
#define PCUT_TEMP_FILENAME_BUFFER_SIZE 128
#define MAX_COMMAND_LINE_LENGTH 1024
#define OUTPUT_BUFFER_SIZE 8192

static char error_message_buffer[OUTPUT_BUFFER_SIZE];
static char extra_output_buffer[OUTPUT_BUFFER_SIZE];

static void before_test_start(pcut_item_t *test) {
	pcut_report_test_start(test);

	memset(error_message_buffer, 0, OUTPUT_BUFFER_SIZE);
	memset(extra_output_buffer, 0, OUTPUT_BUFFER_SIZE);
}


static int convert_wait_status_to_outcome(int status) {
	if (status < 0) {
		return TEST_OUTCOME_ERROR;
	} else if (status == 0) {
		return TEST_OUTCOME_PASS;
	} else {
		return TEST_OUTCOME_FAIL;
	}
}

void pcut_run_test_forking(const char *self_path, pcut_item_t *test) {
	before_test_start(test);

	char tempfile_name[PCUT_TEMP_FILENAME_BUFFER_SIZE];
	snprintf(tempfile_name, PCUT_TEMP_FILENAME_BUFFER_SIZE - 1, "pcut_%d.tmp", getpid());
	FILE *tempfile = fopen(tempfile_name, "w+b");
	if (tempfile == NULL) {
		pcut_report_test_done(test, TEST_OUTCOME_ERROR, "Failed to create temporary file.", NULL, NULL);
		return;
	}

	char command[PCUT_COMMAND_LINE_BUFFER_SIZE];
	snprintf(command, PCUT_COMMAND_LINE_BUFFER_SIZE - 1, "%s -t%d &> %s", \
				self_path, (test)->id, tempfile_name);

	int rc = system(command);
	rc = convert_wait_status_to_outcome(rc);

	fread(extra_output_buffer, 1, OUTPUT_BUFFER_SIZE, tempfile);
	fclose(tempfile);
	remove(tempfile_name);

	pcut_report_test_done_unparsed(test, rc, extra_output_buffer, OUTPUT_BUFFER_SIZE);
}


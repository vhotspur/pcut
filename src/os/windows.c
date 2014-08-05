/*
 * Copyright (c) 2014 Vojtech Horky
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
 * Windows-specific functions for test execution via the popen() system call.
 */

/*
 * Code inspired by Creating a Child Process with Redirected Input and Output:
 * http://msdn.microsoft.com/en-us/library/ms682499%28VS.85%29.aspx
 */

#include "../internal.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>


/** Maximum size of stdout we are able to capture. */
#define OUTPUT_BUFFER_SIZE 8192

/** Maximum command-line length. */
#define PCUT_COMMAND_LINE_BUFFER_SIZE 256

/** Buffer for assertion and other error messages. */
static char error_message_buffer[OUTPUT_BUFFER_SIZE];

/** Buffer for stdout from the test. */
static char extra_output_buffer[OUTPUT_BUFFER_SIZE];

/** Prepare for a new test.
 *
 * @param test Test that is about to be run.
 */
static void before_test_start(pcut_item_t *test) {
	pcut_report_test_start(test);

	memset(error_message_buffer, 0, OUTPUT_BUFFER_SIZE);
	memset(extra_output_buffer, 0, OUTPUT_BUFFER_SIZE);
}

/** Report that a certain function failed.
 *
 * @param test Current test.
 * @param failed_function_name Name of the failed function.
 */
static void report_func_fail(pcut_item_t *test, const char *failed_function_name) {
	/* TODO: get error description. */
	sprintf_s(error_message_buffer, OUTPUT_BUFFER_SIZE - 1,
		"%s failed: %s.", failed_function_name, "unknown reason");
	pcut_report_test_done(test, TEST_OUTCOME_ERROR, error_message_buffer, NULL, NULL);
}

/** Read full buffer from given file descriptor.
 *
 * This function exists to overcome the possibility that read() may
 * not fill the full length of the provided buffer even when EOF is
 * not reached.
 *
 * @param fd Opened file descriptor.
 * @param buffer Buffer to store data into.
 * @param buffer_size Size of the @p buffer in bytes.
 * @return Number of actually read bytes.
 */
static size_t read_all(HANDLE fd, char *buffer, size_t buffer_size) {
	DWORD actually_read;
	char *buffer_start = buffer;
	BOOL okay = FALSE;

	do {
		okay = ReadFile(fd, buffer, buffer_size, &actually_read, NULL);
		if (!okay) {
			break;
		}
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

/** Run the test as a new process and report the result.
 *
 * @param self_path Path to itself, that is to current binary.
 * @param test Test to be run.
 */
void pcut_run_test_forking(const char *self_path, pcut_item_t *test) {
	/* TODO: clean-up if something goes wrong "in the middle" */
	BOOL okay = FALSE;
	DWORD rc;
	int outcome;
	SECURITY_ATTRIBUTES security_attributes;
	HANDLE link_stdout[2] = { NULL, NULL };
	HANDLE link_stderr[2] = { NULL, NULL };
	HANDLE link_stdin[2] = { NULL, NULL };
	size_t stderr_size = 0;
	PROCESS_INFORMATION process_info;
	STARTUPINFO start_info;
	char command[PCUT_COMMAND_LINE_BUFFER_SIZE];

	before_test_start(test);

	/* Pipe handles are inherited. */
	security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	security_attributes.bInheritHandle = TRUE;
	security_attributes.lpSecurityDescriptor = NULL;

	/* Create pipe for stdout, make sure it is not inherited. */
	okay = CreatePipe(&link_stdout[0], &link_stdout[1], &security_attributes, 0);
	if (!okay) {
		report_func_fail(test, "CreatePipe(/* stdout */)");
		return;
	}
	okay = SetHandleInformation(link_stdout[0], HANDLE_FLAG_INHERIT, 0);
	if (!okay) {
		report_func_fail(test, "SetHandleInformation(/* stdout */)");
		return;
	}

	/* Create pipe for stderr, make sure it is not inherited. */
	okay = CreatePipe(&link_stderr[0], &link_stderr[1], &security_attributes, 0);
	if (!okay) {
		report_func_fail(test, "CreatePipe(/* stderr */)");
		return;
	}
	okay = SetHandleInformation(link_stderr[0], HANDLE_FLAG_INHERIT, 0);
	if (!okay) {
		report_func_fail(test, "SetHandleInformation(/* stderr */)");
		return;
	}

	/* Create pipe for stdin, make sure it is not inherited. */
	okay = CreatePipe(&link_stdin[0], &link_stdin[1], &security_attributes, 0);
	if (!okay) {
		report_func_fail(test, "CreatePipe(/* stdin */)");
		return;
	}
	okay = SetHandleInformation(link_stdin[1], HANDLE_FLAG_INHERIT, 0);
	if (!okay) {
		report_func_fail(test, "SetHandleInformation(/* stdin */)");
		return;
	}

	/* Prepare information for the child process. */
	ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&start_info, sizeof(STARTUPINFO));
	start_info.cb = sizeof(STARTUPINFO);
	start_info.hStdError = link_stderr[1];
	start_info.hStdOutput = link_stdout[1];
	start_info.hStdInput = link_stdin[0];
	start_info.dwFlags |= STARTF_USESTDHANDLES;

	/* Format the command line. */
	sprintf_s(command, PCUT_COMMAND_LINE_BUFFER_SIZE - 1,
		"\"%s\" -t%d", self_path, test->id);

	/* Run the process. */
	okay = CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL,
		&start_info, &process_info);

	if (!okay) {
		report_func_fail(test, "CreateProcess()");
		return;
	}

	/* Close handles to the first thread. */
	CloseHandle(process_info.hThread);

	/* Close the other ends of the pipes. */
	okay = CloseHandle(link_stdout[1]);
	if (!okay) {
		report_func_fail(test, "CloseHandle(/* stdout */)");
		return;
	}
	okay = CloseHandle(link_stderr[1]);
	if (!okay) {
		report_func_fail(test, "CloseHandle(/* stderr */)");
		return;
	}
	okay = CloseHandle(link_stdin[0]);
	if (!okay) {
		report_func_fail(test, "CloseHandle(/* stdin */)");
		return;
	}

	/* Read data from stdout and stderr. */
	stderr_size = read_all(link_stderr[0], extra_output_buffer, OUTPUT_BUFFER_SIZE - 1);
	read_all(link_stdout[0], extra_output_buffer, OUTPUT_BUFFER_SIZE - 1 - stderr_size);

	/* Wait for the process to terminate. */
	rc = WaitForSingleObject(process_info.hProcess, INFINITE);
	if (rc != WAIT_OBJECT_0) {
		report_func_fail(test, "WaitForSingleObject(/* PROCESS_INFORMATION.hProcess */)");
		return;
	}

	/* Get the return code and convert it to outcome. */
	okay = GetExitCodeProcess(process_info.hProcess, &rc);
	if (!okay) {
		report_func_fail(test, "GetExitCodeProcess()");
		return;
	}

	if (rc == 0) {
		outcome = TEST_OUTCOME_PASS;
	} else if ((rc > 0) && (rc < 10)) {
		outcome = TEST_OUTCOME_FAIL;
	} else {
		outcome = TEST_OUTCOME_ERROR;
	}

	pcut_report_test_done_unparsed(test, outcome, extra_output_buffer, OUTPUT_BUFFER_SIZE);
}


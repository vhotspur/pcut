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
#include <setjmp.h>
#include <stdarg.h>

#define MAX_MESSAGE_LENGTH 256
#define MESSAGE_BUFFER_COUNT 2
static char message_buffer[MESSAGE_BUFFER_COUNT][MAX_MESSAGE_LENGTH + 1];
static int message_buffer_index = 0;

void pcut_failed_assertion(const char *message) {
#ifndef PCUT_NO_LONG_JUMP
	pcut_bad_test_message = message;
	longjmp(pcut_bad_test_jmp, 1);
#else
	static int teardown_after_failure = 0;

	/*
	 * Check in which phase of a test (setup, test, teardown) we are
	 * currently in.
	 * If the failed assertion was part of setup/test, we need to run the
	 * teardown as well.
	 */
	if (pcut_running_setup_now) {
		printf("%c%s\n", 0, message);
		pcut_running_setup_now = 0;
	} else if (pcut_running_test_now) {
		printf("%c%s\n", 0, message);
		pcut_running_test_now = 0;
	} else {
		/* We are already in a tear-down function. */
		if (!teardown_after_failure) {
			printf("%c", 0);
		}
		printf("%s\n", message);
		exit(1);
	}

	/*
	 * We printed the error message. It is time to run the teardown
	 * function. If we return here, teardown was okay and we terminate.
	 * If the teardown fails as well, we would exit the program in the
	 * else-branch above.
	 */
	teardown_after_failure = 1;
	if (pcut_current_suite->suite.teardown != NULL) {
		pcut_current_suite->suite.teardown();
	}
	exit(1);
#endif
}


void pcut_failed_assertion_fmt(const char *fmt, ...) {
	char *current_buffer = message_buffer[message_buffer_index];
	message_buffer_index = (message_buffer_index + 1) % MESSAGE_BUFFER_COUNT;

	va_list args;
	va_start(args, fmt);
	vsnprintf(current_buffer, MAX_MESSAGE_LENGTH, fmt, args);
	va_end(args);

	pcut_failed_assertion(current_buffer);
}


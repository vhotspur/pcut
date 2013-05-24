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

#include "helper.h"
#include <setjmp.h>
#include <stdarg.h>

#define MAX_MESSAGE_LENGTH 256
static char message_buffer[MAX_MESSAGE_LENGTH + 1];

void pcut_failed_assertion(const char *message) {
	pcut_bad_test_message = message;
	longjmp(pcut_bad_test_jmp, 1);
}


void pcut_failed_assertion_fmt(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsnprintf(message_buffer, MAX_MESSAGE_LENGTH, fmt, args);
	va_end(args);

	pcut_failed_assertion(message_buffer);
}

int pcut_str_equals(const char *a, const char *b) {

#if defined(PCUT_OS_UNIX)

	return strcmp(a, b) == 0;

#elif defined(PCUT_OS_HELENOS)

	return str_cmp(a, b) == 0;

#else

#error Unsupported operation.

#endif
}

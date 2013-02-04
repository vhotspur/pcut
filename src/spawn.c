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

#ifdef PCUT_OS_UNIX
#	include <sys/wait.h>
#endif

#define MAX_COMMAND_LINE_LENGTH 1024

#if defined(PCUT_OS_STDC)

static char command_line_buffer[MAX_COMMAND_LINE_LENGTH];

static int os_respawn(const char *app_path, const char *argument,
		int *normal_exit, int *exit_code) {

	snprintf(command_line_buffer, MAX_COMMAND_LINE_LENGTH, "%s %s", app_path,
		argument);

	int status = system(command_line_buffer);

	if (status == -1) {
		return errno;
	}

#ifdef PCUT_OS_UNIX

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

#endif

	return ENOSYS;
}




#elif defined(PCUT_OS_HELENOS)

static int os_respawn(const char *app_path, const char *argument,
		int *normal_exit, int *exit_code) {
	return ENOTSUP;
}

#else

#error Unsupported operation.

#endif



int pcut_respawn(const char *app_path, const char *argument,
		int *normal_exit, int *exit_code) {
	return os_respawn(app_path, argument, normal_exit, exit_code);
}

/*
 * Add `or' condition for other OSes that do support the `system()'
 * function.
 */

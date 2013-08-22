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

#ifndef PCUT_IMPLCONF_H_GUARD
#define PCUT_IMPLCONF_H_GUARD

#include <pcut/test.h>

#define PCUT_FORKING_METHOD_UNIX_FORK
#undef PCUT_FORKING_METHOD_SYSTEM_WITH_TEMPFILE


#if defined(PCUT_FORKING_METHOD_SYSTEM_WITH_TEMPFILE)
#define PCUT_TEMP_FILENAME_BUFFER_SIZE 128
#define PCUT_TEMP_FILENAME_CREATE(buffer) \
	snprintf(buffer, PCUT_TEMP_FILENAME_BUFFER_SIZE - 1, "pcut_%d.tmp", getpid())

#define PCUT_COMMAND_LINE_BUFFER_SIZE 256
#define PCUT_COMMAND_LINE_CREATE_TEST_RUN(buffer, test, prog_path, temp_filename) \
	snprintf(buffer, PCUT_COMMAND_LINE_BUFFER_SIZE - 1, "%s -t%d &> %s", \
			prog_path, (test)->id, temp_filename)
#endif

#endif

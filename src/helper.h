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

#ifndef PCUT_HELPER_H_GUARD
#define PCUT_HELPER_H_GUARD

#include <pcut/test.h>
#include <stdlib.h>

#define PCUT_UNUSED(x)

jmp_buf pcut_bad_test_jmp;
extern const char *pcut_bad_test_message;

pcut_item_t *pcut_get_real_next(pcut_item_t *item);
pcut_item_t *pcut_get_real(pcut_item_t *item);
const char* pcut_run_test(pcut_test_func_t function);
const char* pcut_run_setup_teardown(pcut_setup_func_t function);
void pcut_print_tests(pcut_item_t *first);
int pcut_respawn(const char *app_path, const char *arg, int *normal_exit, int *exit_code);
int pcut_is_arg_with_number(const char *arg, const char *opt, int *value);

int pcut_run_test_safe(const char *self_path, pcut_item_t *test,
		char **error_message, char **extra_output);
void pcut_run_test_safe_clean(char *error_message, char *extra_output);

#if defined(PCUT_OS_STDC)

#include <string.h>

static inline int pcut_str_start_equals(const char *a, const char *b, int len) {
	return strncmp(a, b, len) == 0;
}

static inline int pcut_str_size(const char *s) {
	return strlen(s);
}

static inline int pcut_str_to_int(const char *s) {
	return atoi(s);
}

#elif defined(PCUT_OS_HELENOS)

static inline int pcut_str_start_equals(const char *a, const char *b, int len) {
	return str_lcmp(a, b, len) == 0;
}

static inline int pcut_str_size(const char *s) {
	return str_size(s);
}

static inline int pcut_str_to_int(const char *s) {
	int result = strtol(s, NULL, 10);
	printf("pcut_str_to_int('%s') = %d\n", s, result);
	return result;
}

#else

#	error Unsupported operation.

#endif


#endif

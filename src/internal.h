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

#ifndef PCUT_INTERNAL_H_GUARD
#define PCUT_INTERNAL_H_GUARD

#include <pcut/test.h>
#include <stdlib.h>

#define PCUT_UNUSED(x) ((void)x)

#define PCUT_RUN_MODE_FORKING 1
#define PCUT_RUN_MODE_SINGLE 2

#define TEST_OUTCOME_PASS 1
#define TEST_OUTCOME_FAIL 2
#define TEST_OUTCOME_ERROR 3

extern int pcut_run_mode;

pcut_item_t *pcut_get_real_next(pcut_item_t *item);
pcut_item_t *pcut_get_real(pcut_item_t *item);
const char* pcut_run_test(pcut_test_func_t function);
const char* pcut_run_setup_teardown(pcut_setup_func_t function);
void pcut_print_tests(pcut_item_t *first);
int pcut_is_arg_with_number(const char *arg, const char *opt, int *value);

void pcut_run_test_forking(const char *self_path, pcut_item_t *test);
int pcut_run_test_forked(pcut_item_t *test);
int pcut_run_test_single(pcut_item_t *test);

extern pcut_item_t *pcut_current_test;
extern pcut_item_t *pcut_current_suite;
extern int pcut_running_test_now;
extern int pcut_running_setup_now;

void pcut_print_fail_message(const char *msg);

void pcut_report_init(pcut_item_t *all_items);
void pcut_report_suite_start(pcut_item_t *suite);
void pcut_report_suite_done(pcut_item_t *suite);
void pcut_report_test_start(pcut_item_t *test);
void pcut_report_test_done(pcut_item_t *test, int outcome,
		const char *error_message, const char *teardown_error_message,
		const char *extra_output);
void pcut_report_test_done_unparsed(pcut_item_t *test, int outcome,
		const char *unparsed_output, size_t unparsed_output_size);
void pcut_report_done(void);


int pcut_str_start_equals(const char *a, const char *b, int len);
int pcut_str_size(const char *s);
int pcut_str_to_int(const char *s);
char *pcut_str_find_char(const char *haystack, const char needle);


#endif

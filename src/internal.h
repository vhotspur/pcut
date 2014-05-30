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

/** @file
 * Common definitions internally used in PCUT.
 */

#ifndef PCUT_INTERNAL_H_GUARD
#define PCUT_INTERNAL_H_GUARD

#include <pcut/pcut.h>
#include <stdlib.h>

/** Mark a variable as unused. */
#define PCUT_UNUSED(x) ((void)x)

/** Forking mode for test execution.
 *
 * In this mode, each test is run in a separate process.
 * This ensures that even SIGSEGV does not stop the framework itself.
 */
#define PCUT_RUN_MODE_FORKING 1

/** Single-process mode for test execution.
 *
 * This mode is used when new process is launched when in forking-mode or
 * this mode can be used if we are sure that no test would fail
 * fatally (that is causing an unexpected program exit).
 */
#define PCUT_RUN_MODE_SINGLE 2

/** Test outcome: test passed. */
#define TEST_OUTCOME_PASS 1

/** Test outcome: test failed. */
#define TEST_OUTCOME_FAIL 2

/** Test outcome: test failed unexpectedly. */
#define TEST_OUTCOME_ERROR 3

extern int pcut_run_mode;


pcut_item_t *pcut_fix_list_get_real_head(pcut_item_t *last);
int pcut_count_tests(pcut_item_t *it);
void pcut_print_items(pcut_item_t *first);

pcut_item_t *pcut_get_real_next(pcut_item_t *item);
pcut_item_t *pcut_get_real(pcut_item_t *item);
void pcut_print_tests(pcut_item_t *first);
int pcut_is_arg_with_number(const char *arg, const char *opt, int *value);

void pcut_run_test_forking(const char *self_path, pcut_item_t *test);
int pcut_run_test_forked(pcut_item_t *test);
int pcut_run_test_single(pcut_item_t *test);

int pcut_get_test_timeout(pcut_item_t *test);

void pcut_failed_assertion(const char *message);
void pcut_print_fail_message(const char *msg);

/** Reporting callbacks structure. */
typedef struct pcut_report_ops pcut_report_ops_t;

/** @copydoc pcut_report_ops_t */
struct pcut_report_ops {
	/** Initialize the reporting, given all tests. */
	void (*init)(pcut_item_t *);
	/** Test suite just started. */
	void (*suite_start)(pcut_item_t *);
	/** Test suite completed. */
	void (*suite_done)(pcut_item_t *);
	/** Test is about to start. */
	void (*test_start)(pcut_item_t *);
	/** Test completed. */
	void (*test_done)(pcut_item_t *, int, const char *, const char *,
		const char *);
	/** Finalize the reporting. */
	void (*done)(void);
};

void pcut_report_register_handler(pcut_report_ops_t *ops);

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

/* OS-dependent functions. */

/** Tell whether two strings start with the same prefix.
 *
 * @param a First string.
 * @param b Second string.
 * @param len Length of common prefix.
 * @return Whether first @p len characters of @p a are the same as in @p b.
 */
int pcut_str_start_equals(const char *a, const char *b, int len);

/** Get size of string in bytes.
 *
 * @param s String in question.
 * @return Size of @p s in bytes.
 */
int pcut_str_size(const char *s);

/** Convert string to integer.
 *
 * @param s String with integer.
 * @return Converted integer.
 */
int pcut_str_to_int(const char *s);

/** Find character in a string.
 *
 * @param haystack Where to look for the @p needle.
 * @param needle Character to find.
 * @return String starting with @p needle.
 * @retval NULL there is no @p needle in @p haystack.
 */
char *pcut_str_find_char(const char *haystack, const char needle);


#endif

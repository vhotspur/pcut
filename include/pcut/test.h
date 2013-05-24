/*
 * Copyright (c) 2012 Vojtech Horky
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

#ifndef PCUT_TEST_H_GUARD
#define PCUT_TEST_H_GUARD

/*
 * First of all, try to guess on which OS we are running.
 * Just the basic, more complex stuff might be added later.
 *
 * Check for HelenOS explicitly as the first one because even
 * the cross-compiler may define the __unix__ macro.
 */
#if defined(__HELENOS__)
#define PCUT_OS_HELENOS
#elif defined(__unix__)
#define PCUT_OS_UNIX
#define PCUT_OS_STDC
#else
#error Ooops, I do not know this system.
#endif

#include <stdio.h>
#include <setjmp.h>
#include <pcut/private.h>

enum {
	PCUT_KIND_SKIP,
	PCUT_KIND_NESTED,
	PCUT_KIND_TESTSUITE,
	PCUT_KIND_TEST
};

void pcut_failed_assertion(const char *message);
void pcut_failed_assertion_fmt(const char *fmt, ...);
int pcut_str_equals(const char *a, const char *b);

typedef struct pcut_item pcut_item_t;
typedef void (*pcut_test_func_t)(void);

struct pcut_item {
	pcut_item_t *previous;
	pcut_item_t *next;
	int id;
	int kind;
	union {
		struct {
			const char *name;
		} suite;
		struct {
			const char *name;
			pcut_test_func_t func;
		} test;
		struct {
			pcut_item_t *last;
		} nested;
		struct {
			int dummy;
		} meta;
	};
};

#define PCUT_ASSERT_EQUALS(expected, actual) \
		do {\
			if (!((expected) == (actual))) { \
				PCUT_ASSERTION_FAILED("Expected <"#expected "> but got <" #actual ">"); \
			} \
		} while (0)


#define PCUT_ASSERT_NULL(pointer) \
	do { \
		void *pcut_ptr_eval = (pointer); \
		if (pcut_ptr_eval != (NULL)) { \
			PCUT_ASSERTION_FAILED("Expected <" #pointer "> to be NULL, " \
				"instead it points to %p", pcut_ptr_eval); \
		} \
	} while (0)

#define PCUT_ASSERT_NOT_NULL(pointer) \
	do { \
		void *pcut_ptr_eval = (pointer); \
		if (pcut_ptr_eval == (NULL)) { \
			PCUT_ASSERTION_FAILED("Pointer <" #pointer "> ought not to be NULL"); \
		} \
	} while (0)


#define PCUT_ASSERT_INT_EQUALS(expected, actual) \
	do {\
		long pcut_expected_eval = (expected); \
		long pcut_actual_eval = (actual); \
		if (pcut_expected_eval != pcut_actual_eval) { \
			PCUT_ASSERTION_FAILED("Expected <%ld> but got <%ld> (%s != %s)", \
				pcut_expected_eval, pcut_actual_eval, \
				#expected, #actual); \
		} \
	} while (0)

#define PCUT_ASSERT_STR_EQUALS(expected, actual) \
	do {\
		const char *pcut_expected_eval = (expected); \
		const char *pcut_actual_eval = (actual); \
		if (!pcut_str_equals(pcut_expected_eval, pcut_actual_eval)) { \
			PCUT_ASSERTION_FAILED("Expected <%s> but got <%s> (%s != %s)", \
				pcut_expected_eval, pcut_actual_eval, \
				#expected, #actual); \
		} \
	} while (0)


#define PCUT_TEST(name) PCUT_TEST_IMPL(name, __COUNTER__)

#define PCUT_TEST_SUITE(name) PCUT_TEST_SUITE_IMPL(name, __COUNTER__)

#define PCUT_EXPORT(identifier) \
	PCUT_EXPORT_IMPL(identifier, __COUNTER__)

#define PCUT_IMPORT(identifier) \
	PCUT_IMPORT_IMPL(identifier, __COUNTER__)

pcut_item_t *pcut_fix_list_get_real_head(pcut_item_t *last);
int pcut_main(pcut_item_t *last, int argc, char *argv[]);
void pcut_print_items(pcut_item_t *first);


#define PCUT_INIT \
	static pcut_item_t PCUT_ITEM_NAME(__COUNTER__) = { \
			.previous = NULL, \
			.next = NULL, \
			.id = -1, \
			.kind = PCUT_KIND_SKIP \
	}; \
	PCUT_TEST_SUITE("Default");

#define PCUT_MAIN \
	static pcut_item_t pcut_item_last = { \
			.previous = &PCUT_JOIN(pcut_item_, PCUT_JOIN(PCUT_PREV_, __COUNTER__)), \
			.kind = PCUT_KIND_SKIP \
	}; \
	int main(int argc, char *argv[]) { \
		return pcut_main(&pcut_item_last, argc, argv); \
	}

#endif

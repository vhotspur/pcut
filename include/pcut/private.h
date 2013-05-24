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

#if !defined(PCUT_TEST_H_GUARD) && !defined(PCUT_INTERNAL)
#error "You cannot include this file directly."
#endif

#ifndef PCUT_PRIVATE_H_GUARD
#define PCUT_PRIVATE_H_GUARD

#include <pcut/prevs.h>

#define PCUT_ASSERTION_FAILED(fmt, ...) \
	pcut_failed_assertion_fmt(__FILE__ ":%d: " fmt, __LINE__, ##__VA_ARGS__)

#define PCUT_JOIN_IMPL(a, b) a##b
#define PCUT_JOIN(a, b) PCUT_JOIN_IMPL(a, b)

#define PCUT_ITEM_NAME(number) \
	PCUT_JOIN(pcut_item_, number)

#define PCUT_ITEM_NAME_PREV(number) \
	PCUT_JOIN(pcut_item_, PCUT_JOIN(PCUT_PREV_, number))

#define PCUT_ADD_ITEM(number, itemkind, ...) \
		static pcut_item_t PCUT_ITEM_NAME(number) = { \
				.previous = &PCUT_ITEM_NAME_PREV(number), \
				.next = NULL, \
				.id = -1, \
				.kind = itemkind, \
				__VA_ARGS__ \
		}; \

#define PCUT_TEST_IMPL(testname, number) \
		static void PCUT_JOIN(test_, testname)(void); \
		PCUT_ADD_ITEM(number, PCUT_KIND_TEST, \
				.test = { \
					.name = #testname, \
					.func = PCUT_JOIN(test_, testname) \
				} \
		) \
		void PCUT_JOIN(test_, testname)(void)

#define PCUT_TEST_SUITE_IMPL(suitename, number) \
		PCUT_ADD_ITEM(number, PCUT_KIND_TESTSUITE, \
				.suite = { \
					.name = suitename, \
					.setup = NULL, \
					.teardown = NULL \
				} \
		)

#define PCUT_TEST_BEFORE_IMPL(number) \
		static void PCUT_JOIN(setup_, number)(void); \
		PCUT_ADD_ITEM(number, PCUT_KIND_SETUP, \
				.setup.func = PCUT_JOIN(setup_, number) \
		) \
		void PCUT_JOIN(setup_, number)(void)

#define PCUT_TEST_AFTER_IMPL(number) \
		static void PCUT_JOIN(teardown_, number)(void); \
		PCUT_ADD_ITEM(number, PCUT_KIND_TEARDOWN, \
				.setup.func = PCUT_JOIN(teardown_, number) \
		) \
		void PCUT_JOIN(teardown_, number)(void)

#define PCUT_EXPORT_IMPL(identifier, number) \
	pcut_item_t pcut_exported_##identifier = { \
		.previous = &PCUT_ITEM_NAME_PREV(number), \
		.next = NULL, \
		.kind = PCUT_KIND_SKIP \
	}

/*
 * Trick with [] and & copied from
 * http://bytes.com/topic/c/answers/553555-initializer-element-not-constant#post2159846
 * because following does not work:
 * extern int *a;
 * int *b = a;
 */
#define PCUT_IMPORT_IMPL(identifier, number) \
	extern pcut_item_t pcut_exported_##identifier; \
	PCUT_ADD_ITEM(number, PCUT_KIND_NESTED, \
		.nested.last = &pcut_exported_##identifier \
	)

#ifdef PCUT_DEBUG_BUILD
#define PCUT_DEBUG(msg, ...) \
	printf("[PCUT]: Debug: " msg "\n", ##__VA_ARGS__)
#else
#define PCUT_DEBUG(msg, ...) (void)0
#endif

#endif

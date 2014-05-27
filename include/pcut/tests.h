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
 * Tests and test suites.
 *
 * @defgroup tests Tests
 * Create test suites and test cases.
 * @{
 */
#ifndef PCUT_TESTS_H_GUARD
#define PCUT_TESTS_H_GUARD

#include <pcut/datadef.h>

/*
 * Trick with [] and & copied from
 * http://bytes.com/topic/c/answers/553555-initializer-element-not-constant#post2159846
 * because following does not work:
 * extern int *a;
 * int *b = a;
 */



/** Default timeout for a single test (in seconds).
 * @showinitializer
 */
#define PCUT_DEFAULT_TEST_TIMEOUT 3





/*
 * Helper macros
 * -------------
 */

/** @cond devel */

/** Join the two arguments on preprocessor level (inner call). */
#define PCUT_JOIN_IMPL(a, b) a##b

/** Join the two arguments on preprocessor level. */
#define PCUT_JOIN(a, b) PCUT_JOIN_IMPL(a, b)

/** Produce identifier name for an item with given number.
 *
 * @param number Item number.
 */
#define PCUT_ITEM_NAME(number) \
	PCUT_JOIN(pcut_item_, number)

/** Produce identifier name for a preceding item.
 *
 * @param number Number of the current item.
 */
#define PCUT_ITEM_NAME_PREV(number) \
	PCUT_JOIN(pcut_item_, PCUT_JOIN(PCUT_PREV_, number))

/** Create a new item, append it to the list.
 *
 * @param number Number of this item.
 * @param itemkind Kind of this item (PCUT_KIND_*).
 * @param ... Other initializers of the pcut_item_t.
 */
#define PCUT_ADD_ITEM(number, itemkind, ...) \
		static pcut_item_t PCUT_ITEM_NAME(number) = { \
				.previous = &PCUT_ITEM_NAME_PREV(number), \
				.next = NULL, \
				.id = -1, \
				.kind = itemkind, \
				__VA_ARGS__ \
		};

/** @endcond */



/*
 * Test-case related macros
 * ------------------------
 */

/** Define test time-out.
 *
 * Use as argument to PCUT_TEST().
 *
 * @param time_out Time-out value in seconds.
 */
#define PCUT_TEST_SET_TIMEOUT(time_out) \
	{ .type = PCUT_EXTRA_TIMEOUT, .timeout = (time_out) }

/** Skip current test.
 *
 * Use as argument to PCUT_TEST().
 */
#define PCUT_TEST_SKIP \
	{ .type = PCUT_EXTRA_SKIP }


/** @cond devel */

/** Terminate list of extra test options. */
#define PCUT_TEST_EXTRA_LAST { .type = PCUT_EXTRA_LAST }

/** Define a new test with given name and given item number.
 *
 * @param testname A valid C identifier name (not quoted).
 * @param number Number of the item describing this test.
 * @param ... Extra test properties.
 */
#define PCUT_TEST_WITH_NUMBER(testname, number, ...) \
		static pcut_extra_t PCUT_JOIN(test_extras_, number)[] = { \
				__VA_ARGS__ \
		}; \
		static void PCUT_JOIN(test_, testname)(void); \
		PCUT_ADD_ITEM(number, PCUT_KIND_TEST, \
				.test = { \
					.name = #testname, \
					.func = PCUT_JOIN(test_, testname), \
					.extras = PCUT_JOIN(test_extras_, number), \
				} \
		) \
		void PCUT_JOIN(test_, testname)(void)

/** @endcond */

/** Define a new test with given name.
 *
 * @param name A valid C identifier name (not quoted).
 * @param ... Extra test properties.
 */
#define PCUT_TEST(name, ...) \
	PCUT_TEST_WITH_NUMBER(name, __COUNTER__, ##__VA_ARGS__, PCUT_TEST_EXTRA_LAST)





/*
 * Test suite related macros
 * -------------------------
 */

/** @cond devel */

#define PCUT_TEST_SUITE_WITH_NUMBER(suitename, number) \
		PCUT_ADD_ITEM(number, PCUT_KIND_TESTSUITE, \
				.suite = { \
					.name = #suitename, \
					.setup = NULL, \
					.teardown = NULL \
				} \
		)

#define PCUT_TEST_BEFORE_WITH_NUMBER(number) \
		static void PCUT_JOIN(setup_, number)(void); \
		PCUT_ADD_ITEM(number, PCUT_KIND_SETUP, \
				.setup.func = PCUT_JOIN(setup_, number) \
		) \
		void PCUT_JOIN(setup_, number)(void)

#define PCUT_TEST_AFTER_WITH_NUMBER(number) \
		static void PCUT_JOIN(teardown_, number)(void); \
		PCUT_ADD_ITEM(number, PCUT_KIND_TEARDOWN, \
				.setup.func = PCUT_JOIN(teardown_, number) \
		) \
		void PCUT_JOIN(teardown_, number)(void)

/** @endcond */


/** Define a new test suite with given name.
 *
 * All tests following this macro belong to the new suite
 * (up to next occurrence of PCUT_TEST_SUITE).
 *
 */
#define PCUT_TEST_SUITE(name) \
	PCUT_TEST_SUITE_WITH_NUMBER(name, __COUNTER__)

/** Define a set-up function for a test suite.
 *
 * There could be only a single set-up function for each suite.
 */
#define PCUT_TEST_BEFORE \
	PCUT_TEST_BEFORE_WITH_NUMBER(__COUNTER__)

/** Define a tear-down function for a test suite.
 *
 * There could be only a single tear-down function for each suite.
 */
#define PCUT_TEST_AFTER \
	PCUT_TEST_AFTER_WITH_NUMBER(__COUNTER__)





/*
 * Import/export related macros
 * ----------------------------
 */

/** @cond devel */

#define PCUT_EXPORT_WITH_NUMBER(identifier, number) \
	pcut_item_t pcut_exported_##identifier = { \
		.previous = &PCUT_ITEM_NAME_PREV(number), \
		.next = NULL, \
		.kind = PCUT_KIND_SKIP \
	}

#define PCUT_IMPORT_WITH_NUMBER(identifier, number) \
	extern pcut_item_t pcut_exported_##identifier; \
	PCUT_ADD_ITEM(number, PCUT_KIND_NESTED, \
		.nested.last = &pcut_exported_##identifier \
	)

/** @endcond */

/** Export test cases from current file.
 *
 * @param identifier Identifier of this block of tests.
 */
#define PCUT_EXPORT(identifier) \
	PCUT_EXPORT_WITH_NUMBER(identifier, __COUNTER__)

/** Import test cases from a different file.
 *
 * @param identifier Identifier of the tests to import
 * (previously exported with PCUT_EXPORT).
 */
#define PCUT_IMPORT(identifier) \
	PCUT_IMPORT_WITH_NUMBER(identifier, __COUNTER__)





/*
 * PCUT initialization and invocation macros
 * -----------------------------------------
 */

/** @cond devel */

/** Initialize the PCUT testing framework with a first item.
 *
 * @param first_number Number of the first item.
 */
#define PCUT_INIT_WITH_NUMBER(first_number) \
	static pcut_item_t PCUT_ITEM_NAME(first_number) = { \
		.previous = NULL, \
		.next = NULL, \
		.id = -1, \
		.kind = PCUT_KIND_SKIP \
	}; \
	PCUT_TEST_SUITE(Default);

int pcut_main(pcut_item_t *last, int argc, char *argv[]);

#define PCUT_MAIN_WITH_NUMBER(last_number) \
	static pcut_item_t pcut_item_last = { \
		.previous = &PCUT_JOIN(pcut_item_, PCUT_JOIN(PCUT_PREV_, last_number)), \
		.kind = PCUT_KIND_SKIP \
	}; \
	int main(int argc, char *argv[]) { \
		return pcut_main(&pcut_item_last, argc, argv); \
	}

/** @endcond */

/** Initialize the PCUT testing framework. */
#define PCUT_INIT \
	PCUT_INIT_WITH_NUMBER(__COUNTER__)

/** Insert code to run all the tests. */
#define PCUT_MAIN() \
	PCUT_MAIN_WITH_NUMBER(__COUNTER__)


/**
 * @}
 */

#endif

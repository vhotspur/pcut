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
 * @defgroup tests Tests
 * Create test suites and test cases.
 * @{
 */
#ifndef PCUT_TESTS_H_GUARD
#define PCUT_TESTS_H_GUARD

#include <pcut/impl.h>

/** Define a new test with given name.
 *
 * @param name A valid C identifier name (not quoted).
 */
#define PCUT_TEST(name, ...) PCUT_TEST_IMPL(name, __COUNTER__, ##__VA_ARGS__, PCUT_TEST_EXTRA_LAST)

/** Define test time-out.
 *
 * Use as argument to PCUT_TEST().
 *
 * @param time_out Time-out value in seconds.
 */
#define PCUT_TEST_SET_TIMEOUT(time_out) PCUT_TEST_SET_TIMEOUT_IMPL(time_out)

#define PCUT_TEST_SKIP PCUT_TEST_SKIP_IMPL

/** Define a new test suite with given name.
 *
 * All tests following this macro belong to the new suite
 * (up to next occurence of PCUT_TEST_SUITE).
 *
 */
#define PCUT_TEST_SUITE(name) PCUT_TEST_SUITE_IMPL(name, __COUNTER__)

/** Define a set-up function for a test suite.
 *
 * There could be only a single set-up function for each suite.
 */
#define PCUT_TEST_BEFORE \
	PCUT_TEST_BEFORE_IMPL(__COUNTER__)

/** Define a tear-down function for a test suite.
 *
 * There could be only a single tear-down function for each suite.
 */
#define PCUT_TEST_AFTER \
	PCUT_TEST_AFTER_IMPL(__COUNTER__)

/** Export test cases from current file. */
#define PCUT_EXPORT(identifier) \
	PCUT_EXPORT_IMPL(identifier, __COUNTER__)

/** Import test cases from a different file. */
#define PCUT_IMPORT(identifier) \
	PCUT_IMPORT_IMPL(identifier, __COUNTER__)

/** Initialize the PCUT testing framework. */
#define PCUT_INIT \
	PCUT_INIT_IMPL(__COUNTER__)

/** Insert code to run all the tests. */
#define PCUT_MAIN() \
	PCUT_MAIN_IMPL(__COUNTER__)


/** Default timeout for a single test (in seconds). */
#define PCUT_DEFAULT_TEST_TIMEOUT 3

/**
 * @}
 */

#endif

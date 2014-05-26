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

#include <stdio.h>

#define GET_ACTUAL_VALUE() actual_value_eval
#define GET_MATCHER_RESULT_SO_FAR() (result)
#define SET_MATCHER_RESULT(x) result = (x)

#define NOT(matcher) \
	do { \
		matcher; \
		SET_MATCHER_RESULT(!GET_MATCHER_RESULT_SO_FAR()); \
	} while (0)

#define OR(a, b) \
	do { \
		a; \
		if (result == 1) break; \
		b; \
	} while (0)

#define ANY_OF_INTERNAL(type, compare, ...) \
	do { \
		type values[] = { __VA_ARGS__ }; \
		for (int i = 0; i < sizeof(values)/sizeof(type); i++) { \
			if (compare(values[i], GET_ACTUAL_VALUE())) { \
				SET_MATCHER_RESULT(1); \
				break; \
			} \
		} \
	} while (0)

#define COMPARE_INT(a, b) ((a) == (b))
#define ANY_OF_INT(...) \
	ANY_OF_INTERNAL(int, COMPARE_INT, __VA_ARGS__)

/* TODO: specify here a comparison function as well, accessible
 * under COMPARE_WITH_ACTUAL(expected_value) that would return
 * the same values as strcmp().
 *
 * That could work type-safely for all the most common assertions
 * such as check for list of values or check for single value.
 */

#define ASSERT_THAT_INTERNAL(type, format_directive, member_access, actual, matchers_name, ...) \
	do { \
		type GET_ACTUAL_VALUE() = (actual); \
		int temp = 0; \
		int result = 0; \
		__VA_ARGS__; \
		if (!result) { \
			printf("Expected %s is %s but got <" format_directive ">.\n", \
				#actual, matchers_name, GET_ACTUAL_VALUE() member_access); \
		} \
	} while (0)

#define ASSERT_THAT_INT(actual, matchers) \
	ASSERT_THAT_INTERNAL(int, "%d", , actual, #matchers, matchers)

#define IS_ODD() \
	SET_MATCHER_RESULT((GET_ACTUAL_VALUE() % 2) == 0);

#define IS_INT(what) \
	SET_MATCHER_RESULT((GET_ACTUAL_VALUE()) == (what));


#define ANY_OF_CMP(x) \
	if ((x) == GET_ACTUAL_VALUE()) { SET_MATCHER_RESULT(1); break; }

#define ANY_OF2(a, b, c, d, e, ...) \
	do { \
		ANY_OF_CMP(a); \
		ANY_OF_CMP(b); \
		ANY_OF_CMP(c); \
		ANY_OF_CMP(d); \
		ANY_OF_CMP(e); \
	} while (0)

#define ANY_OF(x, ...) \
	ANY_OF2(x, __VA_ARGS__, x, x, x, x, x, x, x)

#define EQUALS(what) \
	SET_MATCHER_RESULT((GET_ACTUAL_VALUE()) == (what));


int main() {
	int x = 40 + 2;


	/* Matcher where type specification is necessary. */

	ASSERT_THAT_INT(x, ANY_OF_INT(19, 42));
	ASSERT_THAT_INT(x, NOT( ANY_OF_INT(19, 42) ) );

	ASSERT_THAT_INT(x + 1, ANY_OF_INT(19, 42));
	ASSERT_THAT_INT(x + 1, NOT(NOT(ANY_OF_INT(19, 42))));

	ASSERT_THAT_INT(x + 1, OR(ANY_OF_INT(19, 43), ANY_OF_INT(20, 42)));

	/* Matchers do not specify a type. */

	ASSERT_THAT_INT(x + 2, OR(ANY_OF(23, 45, 41), EQUALS(43)));

	ASSERT_THAT_INT(x * 3 + 1, IS_ODD());
	ASSERT_THAT_INT(x * 3 + 1, OR(IS_ODD(), IS_INT(127)));


	/* Custom matcher. */
#define FAVOURITE() SET_MATCHER_RESULT(GET_ACTUAL_VALUE() == 42)

	ASSERT_THAT_INT(x, FAVOURITE());
	ASSERT_THAT_INT(x, NOT(FAVOURITE()));
	ASSERT_THAT_INT(x + 1, FAVOURITE());
	ASSERT_THAT_INT(x + 1, NOT(FAVOURITE()));

	return 0;
}

//PCUT_MAIN()

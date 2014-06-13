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
#include <string.h>

#define PCUT_VAR_ARG_COUNT_INTERNAL(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, n, ...) n

#define PCUT_VAR_ARG_COUNT(...) \
	PCUT_VAR_ARG_COUNT_INTERNAL(__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)


static inline int pcut_comparator_integer(const long long a, const long long b) {
	if (a == b) {
		return 0;
	} else if (a < b) {
		return -1;
	} else {
		return 1;
	}
}

static inline int pcut_comparator_string(const char *a, const char *b) {
	return strcmp(a, b);
}

#define PCUT_ASSERTION_TEXT_BUFFER_LENGTH 1024
typedef struct {
	char text[PCUT_ASSERTION_TEXT_BUFFER_LENGTH];
} pcut_text_buffer_t;

static void pcut_assertion_text_buffer_append(pcut_text_buffer_t *buffer, const char *text) {
	// FIXME: checking
	strcat(buffer->text, text);
}

static void pcut_assertion_text_buffer_append_integer(pcut_text_buffer_t *buffer, long long value) {
	char tmp[48];
	sprintf(tmp, "%lld", value);
	pcut_assertion_text_buffer_append(buffer, tmp);
}

static void pcut_assertion_text_buffer_append_string(pcut_text_buffer_t *buffer, const char *value) {
	pcut_assertion_text_buffer_append(buffer, "\"");
	pcut_assertion_text_buffer_append(buffer, value);
	pcut_assertion_text_buffer_append(buffer, "\"");
}

#define PCUT_ASSERT_THAT_GET_ACTUAL_VALUE() pcut_that_actual_value_eval
#define PCUT_ASSERT_THAT_TEMP_VAR pcut_that_tmp_value_eval
#define PCUT_ASSERT_THAT_COMPARATOR(a, b) pcut_that_comparator((a), (b))
#define PCUT_ASSERT_THAT_GET_MATCHER_RESULT_SO_FAR() (pcut_that_result)
#define PCUT_ASSERT_THAT_SET_MATCHER_RESULT(x) pcut_that_result = (x)
#define PCUT_ASSERT_THAT_DESCRIPTION_BUFFER pcut_that_buffer
#define PCUT_ASSERT_THAT_TYPE_FORMATTER pcut_that_type_formatter
#define PCUT_ASSERT_THAT_APPEND_DESCRIPTION(str) \
	pcut_assertion_text_buffer_append(&PCUT_ASSERT_THAT_DESCRIPTION_BUFFER, str)
#define PCUT_ASSERT_THAT_APPEND_VALUE(value) \
	PCUT_ASSERT_THAT_TYPE_FORMATTER(&PCUT_ASSERT_THAT_DESCRIPTION_BUFFER, value)


#define PCUT_ASSERT_THAT_TYPED(type, comparator, formatter, actual, matcher_name, matcher) \
	do { \
		type PCUT_ASSERT_THAT_GET_ACTUAL_VALUE() = (actual); \
		type PCUT_ASSERT_THAT_TEMP_VAR = PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(); \
		int (*pcut_that_comparator)(type, type) = comparator; \
		void (*PCUT_ASSERT_THAT_TYPE_FORMATTER)(pcut_text_buffer_t *, type) = formatter; \
		int PCUT_ASSERT_THAT_SET_MATCHER_RESULT(1); \
		pcut_text_buffer_t PCUT_ASSERT_THAT_DESCRIPTION_BUFFER; \
		PCUT_ASSERT_THAT_DESCRIPTION_BUFFER.text[0] = 0; \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION("Expected " #actual " is "); \
		matcher; \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION(" but got "); \
		PCUT_ASSERT_THAT_APPEND_VALUE(PCUT_ASSERT_THAT_GET_ACTUAL_VALUE()); \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION(" [" matcher_name "]."); \
		if (!PCUT_ASSERT_THAT_GET_MATCHER_RESULT_SO_FAR()) { \
			printf("ERROR: %s\n", PCUT_ASSERT_THAT_DESCRIPTION_BUFFER.text); \
		} else { \
			printf(" OKAY: %s\n", PCUT_ASSERT_THAT_DESCRIPTION_BUFFER.text); \
		} \
	} while (0)

#define PCUT_IS_EQUAL(value) \
	do { \
		PCUT_ASSERT_THAT_TEMP_VAR = (value); \
		PCUT_ASSERT_THAT_SET_MATCHER_RESULT( \
			PCUT_ASSERT_THAT_COMPARATOR( \
				PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
				PCUT_ASSERT_THAT_TEMP_VAR) \
			== 0); \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION("equal to "); \
		PCUT_ASSERT_THAT_APPEND_VALUE(PCUT_ASSERT_THAT_TEMP_VAR); \
	} while (0)


#define PCUT_IS_NOT(matcher) \
	do { \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION("not ("); \
		matcher; \
		PCUT_ASSERT_THAT_SET_MATCHER_RESULT( \
			!PCUT_ASSERT_THAT_GET_MATCHER_RESULT_SO_FAR() \
		); \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION(")"); \
	} while (0)


/* Outer loop is to create a safe macro. Inner loop is to allow jumps to
 * the end of the macro.
 */

#define PCUT_IS_ANY_OF_NEXT_VALUE(count, current_index, value) \
	if (current_index >= count) break; \
	if (current_index > 0) PCUT_ASSERT_THAT_APPEND_DESCRIPTION(", "); \
	PCUT_ASSERT_THAT_TEMP_VAR = (value); \
	PCUT_ASSERT_THAT_APPEND_VALUE(PCUT_ASSERT_THAT_TEMP_VAR); \
	if (!PCUT_ASSERT_THAT_GET_MATCHER_RESULT_SO_FAR()) { \
		PCUT_ASSERT_THAT_SET_MATCHER_RESULT( \
			PCUT_ASSERT_THAT_COMPARATOR( \
				PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
				PCUT_ASSERT_THAT_TEMP_VAR) \
			== 0); \
	}


#define PCUT_IS_ANY_OF_COUNTED(count, v0, v1, v2, v3, v4, v5, v6, ...) \
	do { \
		PCUT_IS_ANY_OF_NEXT_VALUE(count, 0, v0); \
		PCUT_IS_ANY_OF_NEXT_VALUE(count, 1, v1); \
		PCUT_IS_ANY_OF_NEXT_VALUE(count, 2, v2); \
		PCUT_IS_ANY_OF_NEXT_VALUE(count, 3, v3); \
		PCUT_IS_ANY_OF_NEXT_VALUE(count, 4, v4); \
		PCUT_IS_ANY_OF_NEXT_VALUE(count, 5, v5); \
		PCUT_IS_ANY_OF_NEXT_VALUE(count, 6, v6); \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION(" !!! INTERNAL ERROR (PCUT_IS_ANY_OF too small) !!! "); \
	} while (0)


#define PCUT_IS_ANY_OF(...) \
	do { \
		PCUT_ASSERT_THAT_SET_MATCHER_RESULT(0); \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION("any of { "); \
		PCUT_IS_ANY_OF_COUNTED(PCUT_VAR_ARG_COUNT(__VA_ARGS__), __VA_ARGS__, \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE(), \
			PCUT_ASSERT_THAT_GET_ACTUAL_VALUE()); \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION(" }"); \
	} while (0)


#define PCUT_ASSERT_THAT_INT(actual, matcher) \
	PCUT_ASSERT_THAT_TYPED( \
		long long, pcut_comparator_integer, \
		pcut_assertion_text_buffer_append_integer, \
		actual, #matcher, matcher)

#define PCUT_ASSERT_THAT_STRING(actual, matcher) \
	PCUT_ASSERT_THAT_TYPED( \
		const char *, pcut_comparator_string, \
		pcut_assertion_text_buffer_append_string, \
		actual, #matcher, matcher)


typedef struct {
	int x;
	int y;
} coord_t;

static inline int coord_comparator(const coord_t a, const coord_t b) {
	if ((a.x == b.x) && (a.y == b.y)) {
		return 0;
	} else {
		return -1;
	}
}

static void coord_append_to_buffer(pcut_text_buffer_t *buffer, coord_t value) {
	char tmp[48];
	sprintf(tmp, "[%d,%d]", value.x, value.y);
	strcat(buffer->text, tmp);
}



#define PCUT_ASSERT_THAT_COORD(actual, matcher) \
	PCUT_ASSERT_THAT_TYPED( \
		coord_t, coord_comparator, \
		coord_append_to_buffer, \
		actual, #matcher, matcher)



int main() {
	int x = 40 + 2;


	/* Matcher where type specification is necessary. */

	PCUT_ASSERT_THAT_INT(x, PCUT_IS_EQUAL(42));
	PCUT_ASSERT_THAT_INT(x + 1, PCUT_IS_EQUAL(42 - 2));

	PCUT_ASSERT_THAT_INT(x, PCUT_IS_NOT(PCUT_IS_EQUAL(42)));
	PCUT_ASSERT_THAT_INT(x, PCUT_IS_NOT(PCUT_IS_EQUAL(40)));

	PCUT_ASSERT_THAT_INT(x, PCUT_IS_ANY_OF(4, 5, 23, 42));
	PCUT_ASSERT_THAT_INT(x, PCUT_IS_ANY_OF(4, 5, 23, 12, 42));
	PCUT_ASSERT_THAT_INT(x, PCUT_IS_ANY_OF(4, 5, 23, 41));
	PCUT_ASSERT_THAT_INT(x, PCUT_IS_ANY_OF(4));
	PCUT_ASSERT_THAT_INT(x, PCUT_IS_NOT(PCUT_IS_ANY_OF(4, 5, 23, 42)));

	int y = 41;
	PCUT_ASSERT_THAT_INT(x++, PCUT_IS_EQUAL(++y));

	/* Custom matcher. */
#define FAVOURITE() do { \
		PCUT_ASSERT_THAT_SET_MATCHER_RESULT(PCUT_ASSERT_THAT_GET_ACTUAL_VALUE() == 42); \
		PCUT_ASSERT_THAT_APPEND_DESCRIPTION("favourite"); \
	} while (0)

	PCUT_ASSERT_THAT_INT(x, FAVOURITE());
	PCUT_ASSERT_THAT_INT(x, PCUT_IS_NOT(FAVOURITE()));
	PCUT_ASSERT_THAT_INT(x + 1, FAVOURITE());
	PCUT_ASSERT_THAT_INT(x + 1, PCUT_IS_NOT(FAVOURITE()));


	/* Custom type. */

	coord_t target = { 4, 5 };
	coord_t good = { 4, 5 };
	coord_t bad = { 4, 6 };


	PCUT_ASSERT_THAT_COORD(target, PCUT_IS_EQUAL(good));
	PCUT_ASSERT_THAT_COORD(target, PCUT_IS_EQUAL(bad));
	PCUT_ASSERT_THAT_COORD(target, PCUT_IS_ANY_OF(good, bad));


	/* Strings. */

	const char *greetings = "hello";
	PCUT_ASSERT_THAT_STRING(greetings, PCUT_IS_EQUAL("hello"));
	PCUT_ASSERT_THAT_STRING(greetings, PCUT_IS_EQUAL("hullo"));

	PCUT_ASSERT_THAT_STRING(greetings, PCUT_IS_ANY_OF("hullo", "hello", "hallo"));
	PCUT_ASSERT_THAT_STRING(greetings, PCUT_IS_NOT(PCUT_IS_ANY_OF("hullo", "hello", "hallo")));

	return 0;
}

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

#define PCUT_JOIN_IMPL(a, b) a##b

#define PCUT_JOIN(a, b) PCUT_JOIN_IMPL(a, b)

#define ARG_COUNT_INTERNAL(a, b, c, d, e, n, ...) n

#define ARG_COUNT(...) \
	ARG_COUNT_INTERNAL(__VA_ARGS__, 5, 4, 3, 2, 1)

#define SHOW_ARG_COUNT(...) \
	printf("%s ==> %d\n", #__VA_ARGS__, ARG_COUNT(__VA_ARGS__))

#define CASE_1(x) \
	printf("CASE_1(%d)\n", x)

#define CASE_2(x, y) \
	printf("CASE_2(%d + %d = %d)\n", x, y, (x) + (y))

#define CASE(...) \
	PCUT_JOIN(CASE_, ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

int main() {
	SHOW_ARG_COUNT(x);
	SHOW_ARG_COUNT(x, y);
	SHOW_ARG_COUNT(x, y, z);

	CASE(1);
	CASE(4, 2);

	return 0;
}


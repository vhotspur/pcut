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

#include <pcut/test.h>
#include <stdlib.h>

PCUT_INIT

static char *buffer = NULL;
#define BUFFER_SIZE 512

PCUT_TEST_SUITE("Tests with set-up and tear-down")

PCUT_TEST_BEFORE {
	buffer = malloc(BUFFER_SIZE);
	PCUT_ASSERT_NOT_NULL(buffer);
}

PCUT_TEST_AFTER {
	free(buffer);
	buffer = NULL;
}

PCUT_TEST(snprintf) {
	snprintf(buffer, BUFFER_SIZE - 1, "%d-%s", 56, "abcd");
	PCUT_ASSERT_STR_EQUALS("56-abcd", buffer);
}

PCUT_TEST_SUITE("Another suite without set-up")

PCUT_TEST(whatever) {
	PCUT_ASSERT_NULL(buffer);
}


PCUT_MAIN

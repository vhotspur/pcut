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

#define PCUT_INTERNAL
#include "helper.h"

int pcut_error_count;

int pcut_main(pcut_item_t *last) {
	pcut_item_t *items = pcut_fix_list_get_real_head(last);
	// pcut_print_items(items);

	for (pcut_item_t *it = items; it != NULL; it = pcut_get_real_next(it)) {
		switch (it->kind) {
		case PCUT_KIND_TESTSUITE:
			printf("Running test suite `%s'...\n", it->suite.name);
			break;
		case PCUT_KIND_TEST: {
			printf("   Test `%s': ", it->test.name);

			const char *error_message = pcut_run_test(it->test.func);
			if (error_message == NULL) {
				printf("OK\n");
			} else {
				printf("FAIL\n      %s\n", error_message);
			}
			break;
		}
		default:
			break;
		}
	}

	return 0;
}

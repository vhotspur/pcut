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

#include <stdlib.h>
#include <assert.h>
#include "internal.h"


pcut_item_t *pcut_get_real_next(pcut_item_t *item) {
	if (item == NULL) {
		return NULL;
	}

	do {
		item = item->next;
	} while ((item != NULL) && (item->kind == PCUT_KIND_SKIP));


	return item;
}

pcut_item_t *pcut_get_real(pcut_item_t *item) {
	if (item == NULL) {
		return NULL;
	}

	if (item->kind == PCUT_KIND_SKIP) {
		return pcut_get_real_next(item);
	} else {
		return item;
	}
}

int pcut_is_arg_with_number(const char *arg, const char *opt, int *value) {
	int opt_len = pcut_str_size(opt);
	if (! pcut_str_start_equals(arg, opt, opt_len)) {
		return 0;
	}
	int val = pcut_str_to_int(arg + opt_len);
	*value = val;
	return 1;
}

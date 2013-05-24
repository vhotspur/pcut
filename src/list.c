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

#include <assert.h>
#include <stdlib.h>
#include "helper.h"
#include <pcut/test.h>


static void fix_nested(pcut_item_t *nested) {
	if (nested->kind != PCUT_KIND_NESTED) {
		return;
	}

	if (nested->nested.last == NULL) {
		nested->kind = PCUT_KIND_SKIP;
		return;
	}

	pcut_item_t *first = pcut_fix_list_get_real_head(nested->nested.last);
	nested->nested.last->next = nested->next;
	if (nested->next != NULL) {
		nested->next->previous = nested->nested.last;
	}
	nested->next = first;
	first->previous = nested;

	nested->kind = PCUT_KIND_SKIP;
}

static void set_ids(pcut_item_t *first) {
	assert(first != NULL);
	int id = 1;
	if (first->kind == PCUT_KIND_SKIP) {
		first = pcut_get_real_next(first);
	}
	for (pcut_item_t *it = first; it != NULL; it = pcut_get_real_next(it)) {
		it->id = id;
		id++;
	}
}

pcut_item_t *pcut_fix_list_get_real_head(pcut_item_t *last) {
	last->next = NULL;

	fix_nested(last);

	pcut_item_t *next = last;

	pcut_item_t *it = last->previous;
	while (it != NULL) {
		it->next = next;
		fix_nested(it);
		next = it;
		it = it->previous;
	}

	set_ids(next);

	return next;
}

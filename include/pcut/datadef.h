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

/** @file
 * Data types internally used by PCUT.
 */
#ifndef PCUT_DATADEF_H_GUARD
#define PCUT_DATADEF_H_GUARD

#include <pcut/prevs.h>
#include <stdlib.h>
#include <stddef.h>

/** @cond devel */

enum {
	PCUT_KIND_SKIP,
	PCUT_KIND_NESTED,
	PCUT_KIND_SETUP,
	PCUT_KIND_TEARDOWN,
	PCUT_KIND_TESTSUITE,
	PCUT_KIND_TEST
};

enum {
	PCUT_EXTRA_TIMEOUT,
	PCUT_EXTRA_SKIP,
	PCUT_EXTRA_LAST
};

typedef struct pcut_item pcut_item_t;
typedef struct pcut_extra pcut_extra_t;
typedef void (*pcut_test_func_t)(void);
typedef void (*pcut_setup_func_t)(void);

struct pcut_extra {
	int type;
	union {
		int timeout;
	};
};

struct pcut_item {
	pcut_item_t *previous;
	pcut_item_t *next;
	int id;
	int kind;
	union {
		struct {
			const char *name;
			pcut_setup_func_t setup;
			pcut_setup_func_t teardown;
		} suite;
		struct {
			const char *name;
			pcut_test_func_t func;
			pcut_extra_t *extras;
		} test;
		/* setup is used for both set-up and tear-down */
		struct {
			pcut_setup_func_t func;
		} setup;
		struct {
			pcut_item_t *last;
		} nested;
		struct {
			int dummy;
		} meta;
	};
};

#ifdef PCUT_DEBUG_BUILD
#define PCUT_DEBUG(msg, ...) \
	printf("[PCUT]: Debug: " msg "\n", ##__VA_ARGS__)
#else
#define PCUT_DEBUG(msg, ...) (void)0
#endif

/** @endcond */

#endif

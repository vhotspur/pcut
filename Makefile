#
# Copyright (c) 2012 Vojtech Horky
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# - The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

CC = gcc
LD = gcc
AR = ar
INCLUDES = -Iinclude
CFLAGS = $(INCLUDES) -Wall -Wextra -std=c99
DEPEND = Makefile.depend

SOURCES = src/list.c src/main.c src/print.c src/helper.c src/run.c src/assert.c
OBJECTS = $(addsuffix .o,$(basename $(SOURCES)))

.PHONY: all samples clean

all: libpcut.a

-include $(DEPEND)

$(DEPEND):
	makedepend -f - $(INCLUDES) -- $(SOURCES) >$@ 2>/dev/null

samples: libpcut.a
	$(MAKE) -C tests clean run

clean:
	find -name '*.o' -delete
	rm -f libpcut.a
	rm -f $(DEPEND)
	$(MAKE) -C tests clean

libpcut.a: $(OBJECTS)
	$(AR) rc $@ $(OBJECTS)
	
%.o: %.c $(DEPEND)
	$(CC) -c -o $@ $(CFLAGS) $<


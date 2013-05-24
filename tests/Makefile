#
# Copyright (c) 2012-2013 Vojtech Horky
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
INCLUDES = -I../include
CFLAGS = $(INCLUDES) -Wall -Wextra
LDFLAGS = -L../ -lpcut
DEPEND = Makefile.depend

APP_DEPS = $(APP_OBJECTS)
APP_OBJECTS = tested.o

TEST_APPS = simple.out suites.out multisuite.out null.out manytests.out alloc.out

.PHONY: all run clean

all: $(TEST_APPS)

-include $(DEPEND)

$(DEPEND):
	makedepend -f - $(INCLUDES) -- $(SOURCES) >$@ 2>/dev/null

simple.out: $(APP_DEPS) simple.o
	$(LD) -o $@ $(APP_OBJECTS) simple.o $(LDFLAGS)

suites.out: $(APP_DEPS) suites.o
	$(LD) -o $@ $(APP_OBJECTS) suites.o $(LDFLAGS)
	
multisuite.out: $(APP_DEPS) suite_all.o suite1.o suite2.o
	$(LD) -o $@ $(APP_OBJECTS) suite_all.o suite1.o suite2.o $(LDFLAGS)
	
null.out: $(APP_DEPS) null.o
	$(LD) -o $@ $(APP_OBJECTS) null.o $(LDFLAGS)

manytests.out: $(APP_DEPS) manytests.o
	$(LD) -o $@ $(APP_OBJECTS) manytests.o $(LDFLAGS)

alloc.out: $(APP_DEPS) alloc.o
	$(LD) -o $@ $(APP_OBJECTS) alloc.o $(LDFLAGS)

%.o: %.c $(DEPEND)
	$(CC) -c -o $@ $(CFLAGS) $<


clean:
	rm -f *.out *.o
	rm -f $(DEPEND)

run: $(TEST_APPS)
	@for i in $(TEST_APPS); do echo ./$$i; ./$$i; done
	

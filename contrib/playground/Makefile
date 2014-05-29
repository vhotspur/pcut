
CC = gcc
CFLAGS = -std=c99
LDFLAGS = 

all: assert_that.run

assert_that.run: assert_that.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f assert_that.run
	
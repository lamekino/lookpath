PROG_NAME := lookpath

CC := gcc
CC_FLAGS := -std=c9x -ggdb -pedantic -Wall -Wextra -Werror -D_XOPEN_SOURCE=500

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: $(PROG_NAME)

clean:
	rm -fr $(PROG_NAME) *.o

install:
	install -m 0755 $(PROG_NAME) $(PREFIX)/bin

$(PROG_NAME).o: $(PROG_NAME).c
	$(CC) $(CC_FLAGS) -c -o $@ $^

$(PROG_NAME): $(PROG_NAME).o
	$(CC) -o $@ $^

.PHONY: all clean install

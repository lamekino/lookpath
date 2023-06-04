#!/bin/sh

set -x
gcc -std=c89 -ggdb -pedantic -Wall -Wextra -Werror -D_XOPEN_SOURCE=500 -DDEBUG \
    -lpthread -o lookpath lookpath.c

#!/bin/sh

set -x
gcc -std=c9x -ggdb -pedantic -Wall -Wextra -Werror -D_XOPEN_SOURCE=500 -DDEBUG \
    -lpthread -o lookpath lookpath.c

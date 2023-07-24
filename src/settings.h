#ifndef SETTINGS_H
#define SETTINGS_H
#include <stddef.h>
#include <stdbool.h>

#include "arguments.h"
#include "print_modes.h"
#include "search_methods.h"

typedef struct {
    enum print_mode print_mode;
    enum search_methods strategy;
    char *pattern;
    size_t pattern_len;
} settings_t;

#endif

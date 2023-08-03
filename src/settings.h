#ifndef SETTINGS_H
#define SETTINGS_H
#include <stddef.h>
#include <stdbool.h>

#include "arguments.h"
#include "print_modes.h"
#include "search_methods.h"

#define EMPTY_MASK ((packed_t) 0)
#define MASK(base_enum) 1 << (base_enum)

typedef struct {
    packed_t mask[NUM_CATEGORIES];

    /* TODO: use mask for print_mode */
    enum print_mode print_mode;
    char *pattern;
    size_t pattern_len;
} settings_t;

#endif

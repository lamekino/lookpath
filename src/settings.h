#ifndef SETTINGS_H
#define SETTINGS_H
#include <stddef.h>
#include <stdbool.h>

#include "bits.h"
#include "arguments.h"

typedef struct {
    packed_t mask[NUM_CATEGORIES];

    char *pattern;
    size_t pattern_len;
} settings_t;

#endif

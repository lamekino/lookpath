#ifndef SEARCH_METHODS_H
#define SEARCH_METHODS_H

#include <stddef.h>
#include <stdbool.h>

typedef bool (*search_method_fp)(const char *, const char *, size_t);

enum search_methods {
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT,

    NUM_SEARCH_METHODS
};

search_method_fp get_matcher(enum search_methods method);

#endif

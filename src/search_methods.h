#ifndef SEARCH_METHODS_H
#define SEARCH_METHODS_H

#include "debug_assert.h"
#include "arguments.h"

#include <stddef.h>
#include <stdbool.h>

typedef bool (*search_method_fp)(const char *, const char *, size_t);

enum search_methods {
#define SEARCH_METHOD_ENUM(label, ...) SM_##label,
    QUERY_FLAGS(SEARCH_METHOD_ENUM)
#undef SEARCH_METHOD_ENUM

    NUM_SEARCH_METHODS
};

search_method_fp get_matcher(enum search_methods method);

#endif

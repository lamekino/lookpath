#ifndef SEARCH_METHODS_H
#define SEARCH_METHODS_H

#include "debug_assert.h"
#include "arguments.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(USE_UINT32_T)
typedef uint32_t packed_t;
#elif defined(USE_UINT16_T)
typedef uint16_t packed_t;
#elif defined(USE_UINT8_T)
typedef uint8_t packed_t;
#else
typedef uint64_t packed_t;
#endif

typedef bool (*search_method_fp)(const char *, const char *, size_t);

enum search_methods {
#define SEARCH_METHOD_ENUM(label, ...) SM_##label,
    QUERY_FLAGS(SEARCH_METHOD_ENUM)
#undef SEARCH_METHOD_ENUM

    NUM_SEARCH_METHODS
};

search_method_fp get_matcher(const packed_t qmask);

#endif

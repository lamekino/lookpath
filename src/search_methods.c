#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "search_methods.h"
#include "debug_assert.h"
#include "limits.h"

#define BITS (size_t) 8
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define PACK(dst, a, b, offset) \
    (dst) |= ((uint8_t) ((a) ^ (b)) != 0) << offset

#if defined(USE_UINT32_T)
typedef uint32_t packed_t;
#elif defined(USE_UINT16_T)
typedef uint16_t packed_t;
#elif defined(USE_UINT8_T)
typedef uint8_t packed_t;
#else
typedef uint64_t packed_t;
#endif

static bool matches_pattern(const char *fname,
                            const char *pattern,
                            size_t pattern_len) {
    size_t window = sizeof(packed_t) * BITS;

    for (size_t pos = 0; pos < pattern_len; pos += window) {
        packed_t has_duplicate = 0;

        window = MIN(window, pattern_len - pos);

        for (size_t view = 0; view < window; view++) {
            const char view_fname = fname[pos + view];
            const char view_pattern = pattern[pos + view];

            if (view_fname == '\0') {
                return false;
            }

            PACK(has_duplicate, view_fname, view_pattern, view);
        }

        if (has_duplicate) {
            return false;
        }
    }

    return true;
}

static bool matches_pattern_reverse(const char *fname,
                                    const char *pattern,
                                    size_t pattern_len) {
    const char *cursor_pattern, *cursor_fname;

    const size_t fname_len = strnlen(fname, NAME_MAX + 1);
    size_t window = sizeof(packed_t) * BITS;

    if (fname_len < pattern_len) {
        return false;
    }


    cursor_fname = &fname[fname_len - 1];
    cursor_pattern = &pattern[pattern_len - 1];

    int num_seen = 0;
    while (cursor_pattern >= pattern) {
        packed_t has_duplicate = 0;

        window = MIN(window, pattern_len - num_seen);

        for (size_t view = 0; view < window; view++) {
            /* look backwards in the tail of the list */
            const char view_fname = cursor_fname[-view];
            const char view_pattern = cursor_pattern[-view];

            PACK(has_duplicate, view_fname, view_pattern, view);
        }

        if (has_duplicate) {
            return false;
        }

        cursor_pattern -= window;
        cursor_fname -= window;
        num_seen += window;
    }

    return true;
}


search_method_fp get_matcher(enum search_methods strategy) {
    ASSERT(0 <= strategy && strategy < NUM_SEARCH_METHODS);

    const search_method_fp methods[] = {
        [LEFT_TO_RIGHT] = &matches_pattern,
        [RIGHT_TO_LEFT] = &matches_pattern_reverse
    };
    STATIC_ASSERT(sizeof(methods)/sizeof(methods[0]) == NUM_SEARCH_METHODS,
            missing_matcher_impl);

    return methods[strategy];
}


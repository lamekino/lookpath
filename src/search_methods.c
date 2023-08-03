#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "search_methods.h"
#include "arguments.h"
#include "debug_assert.h"
#include "limits.h"
#include "settings.h"

#define BITS (size_t) 8
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define PACK(dst, a, b, offset) \
    (dst) |= ((uint8_t) ((a) ^ (b)) != 0) << offset

typedef int (*transform_fp)(int);

static bool matches_pattern(const char *fname,
                            const char *pattern,
                            const transform_fp transform,
                            size_t pattern_len) {
    size_t window = sizeof(packed_t) * BITS;

    for (size_t pos = 0; pos < pattern_len; pos += window) {
        packed_t has_duplicate = 0;

        window = MIN(window, pattern_len - pos);

        for (size_t view = 0; view < window; view++) {
            const char view_fname = transform(fname[pos + view]);
            const char view_pattern = transform(pattern[pos + view]);

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
                                    const transform_fp transform,
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
            const char view_fname = transform(cursor_fname[-view]);
            const char view_pattern = transform(cursor_pattern[-view]);

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

static bool insensitive_forward_search(const char *fname,
                                       const char *pattern,
                                       size_t pattern_len) {
    return matches_pattern(fname, pattern, &tolower, pattern_len);
}

static bool insensitive_reverse_search(const char *fname,
                                       const char *pattern,
                                       size_t pattern_len) {
    return matches_pattern_reverse(fname, pattern, &tolower, pattern_len);
}

static int id(int x) {
    return x;
}

static bool sensitive_forward_search(const char *fname,
                                    const char *pattern,
                                    size_t pattern_len) {
    return matches_pattern(fname, pattern, &id, pattern_len);
}

static bool sensitive_reverse_search(const char *fname,
                                    const char *pattern,
                                    size_t pattern_len) {
    return matches_pattern_reverse(fname, pattern, &id, pattern_len);
}

search_method_fp get_matcher(const packed_t qmask) {
    switch (qmask) {
        case EMPTY_MASK:
        case MASK(SM_LEFT_TO_RIGHT):
            return &sensitive_forward_search;
        case MASK(SM_CASE_INSENSITIVE):
        case MASK(SM_CASE_INSENSITIVE) | MASK(SM_LEFT_TO_RIGHT):
            return &insensitive_forward_search;
        case MASK(SM_RIGHT_TO_LEFT):
            return &sensitive_reverse_search;
        case MASK(SM_CASE_INSENSITIVE) | MASK(SM_RIGHT_TO_LEFT):
            return &insensitive_reverse_search;
        default:
            ASSERT(0 && "unreachable");
            return &sensitive_forward_search;
    }
}

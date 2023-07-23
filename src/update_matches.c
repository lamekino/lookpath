#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>

#include "debug_assert.h"
#include "update_matches.h"
#include "settings.h"
#include "errors.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define PACK(dst, a, b, offset) \
    (dst) |= ((uint8_t) ((a) ^ (b)) != 0) << offset

typedef bool (*match_checker)(const char *, const char *, size_t);
#ifdef USE_64BITS
typedef uint64_t packed_t;
#else
typedef uint32_t packed_t;
#endif

static bool matches_pattern(const char *fname,
                            const char *pattern,
                            size_t pattern_len) {
    size_t window = sizeof(packed_t) * 8;

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
    size_t window = sizeof(packed_t) * 8;

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

static match_checker get_matcher(enum order order) {
    ASSERT(0 <= order && order < LEN_ORDER);

    const match_checker matchers[] = {
        [IN_PLACE] = &matches_pattern,
        [REVERSE] = &matches_pattern_reverse
    };
    STATIC_ASSERT(sizeof(matchers)/sizeof(matchers[0]) == LEN_ORDER,
            missing_matcher_impl);

    return matchers[order];
}

static int add_to_matchlist(char ***matches_list,
                            size_t matches_len,
                            char *filename) {
    char *buffer_fname = NULL;
    char **matches_resize =
        realloc(*matches_list, sizeof(*matches_list) * (matches_len + 1));
    if (matches_resize == NULL) {
        return FAILED_BUFFER_REALLOC;
    }

    /* set the buffer_fname to filename */
    buffer_fname = strdup(filename);
    if (buffer_fname == NULL) {
        free(matches_resize);
        return FAILED_BUFFER_ALLOC;
    }

    matches_resize[matches_len] = buffer_fname;
    *matches_list = matches_resize;

    return matches_len + 1;
}

size_t update_matches(size_t head_cursor,
                      char ***matches_list,
                      const settings_t *settings,
                      const char *path) {
    const char *pattern = settings->pattern;
    const size_t pattern_len = settings->pattern_len;
    const match_checker has_matches = get_matcher(settings->order);

    DIR *dir = opendir(path);
    struct dirent *entry = NULL;

    if (dir == NULL) {
        /* when the path doesn't exist in the $PATH, just skip */
        return head_cursor;
    }

    while ((entry = readdir(dir))) {
        if (has_matches(entry->d_name, pattern, pattern_len)) {
            int tail_cursor =
                add_to_matchlist(matches_list, head_cursor, entry->d_name);

            /* TODO: ehehhehhhh */
            if (HAS_ERROR(tail_cursor)) {
                closedir(dir);
                return tail_cursor;
            }

            head_cursor = tail_cursor;
        }
    }
    closedir(dir);

    return head_cursor;
}

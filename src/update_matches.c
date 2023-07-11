#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>

#include "update_matches.h"
#include "errors.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static bool matches_pattern(const char *fname,
                            const char *pattern,
                            size_t pattern_len) {
    size_t window = sizeof(uint64_t);

    for (size_t pos = 0; pos < pattern_len; pos += window) {
        uint64_t has_duplicate = 0;

        window = MIN(sizeof(uint64_t), pattern_len - pos);

        for (size_t view = 0; view < window; view++) {
            const char *view_fname = &fname[pos + view];
            const char *view_pattern = &pattern[pos + view];

            if (*view_fname == '\0') {
                return false;
            }

            has_duplicate |=
                ((uint8_t) *view_fname ^ *view_pattern) << 8 * view;
        }

        if (has_duplicate) {
            return false;
        }
    }

    return true;
}

/* PERF: pls optimize */
/* - TODO: use geometric resize */
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

int update_matches(const char *path,
                   const char *pattern,
                   const size_t pattern_len,
                   char ***matches_list,
                   int num_matches) {
    DIR *dir = opendir(path);
    struct dirent *entry = NULL;

    if (dir == NULL) {
        /* when the path doesn't exist in the $PATH, just skip */
        return num_matches;
    }

    while ((entry = readdir(dir))) {
        if (matches_pattern(entry->d_name, pattern, pattern_len)) {
            int end_of_matches =
                add_to_matchlist(matches_list, num_matches, entry->d_name);
            if (end_of_matches < 0) {
                closedir(dir);
                return end_of_matches;
            }

            num_matches = end_of_matches;
        }
    }
    closedir(dir);

    return num_matches;
}

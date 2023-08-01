#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>

#include "debug_assert.h"
#include "search_methods.h"
#include "update_matches.h"
#include "settings.h"
#include "errors.h"

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
    const search_method_fp has_matches = get_matcher(settings->strategy);

    DIR *dir = opendir(path);
    struct dirent *entry = NULL;

    if (dir == NULL) {
        /* when the path doesn't exist in the $PATH, just skip */
        return head_cursor;
    }

    /* TODO: look into scandir(3) */
    while ((entry = readdir(dir))) {
        if (has_matches(entry->d_name, pattern, pattern_len)) {
            int tail_cursor =
                add_to_matchlist(matches_list, head_cursor, entry->d_name);

            /* FIXME: error expects integer value but this return is unsigned */
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

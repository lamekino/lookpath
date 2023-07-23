#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tag_matches.h"
#include "update_matches.h"
#include "tagged_list.h"
#include "errors.h"

/* NOTE: i have no clue if this works on windows */
#ifdef __WIN32
#define PATH_SEPARATOR ";"
#else
#define PATH_SEPARATOR ":"
#endif

int tag_matches(const settings_t *settings,
                tagged_list *tags,
                char *env_path) {
    int head_cursor = 0;

    char *path, *remaining;
    for (
            path = strtok_r(env_path, PATH_SEPARATOR, &remaining);
            path != NULL;
            path = strtok_r(NULL, PATH_SEPARATOR, &remaining)
    ) {
        tagged_list *resized_tags = NULL;
        int tail_cursor = head_cursor;
        int new_cursor = /* tracks the end of the current tag */
            update_matches(head_cursor,
                           &tags->strings,
                           settings,
                           path);

        if (new_cursor < 0) {
            return new_cursor;
        }

        /* don't alloc things with no matches */
        if (tail_cursor == new_cursor) {
            continue;
        }

        tail_cursor = new_cursor;
        resized_tags = add_tag(tags, path, head_cursor, tail_cursor);
        if (resized_tags == NULL) {
            return FAILED_BUFFER_REALLOC;
        }
        tags = resized_tags;

        tags->num_strings = tail_cursor;
        head_cursor = tail_cursor;
    }

    return NO_ERROR;
}


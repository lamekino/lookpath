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

int tag_matches(char *env_path,
                const char *pattern,
                tagged_list *tags) {
    size_t pattern_len = strlen(pattern);

    size_t num_tags = 0;
    int num_matches = 0;

    char *path, *remaining;
    for (
            path = strtok_r(env_path, PATH_SEPARATOR, &remaining);
            path != NULL;
            path = strtok_r(NULL, PATH_SEPARATOR, &remaining)
    ) {
        int end_of_matches = 0;
        tagged_list *resized_tags = NULL;

        end_of_matches =
            update_matches(path, pattern, pattern_len, &tags->strings, num_matches);
        if (end_of_matches < 0) {
            return end_of_matches;
        }

        resized_tags = add_tag(tags, path, num_matches, end_of_matches);
        if (resized_tags == NULL) {
            return FAILED_BUFFER_REALLOC;
        }
        tags = resized_tags;
        tags->num_strings = end_of_matches;
        num_matches = end_of_matches;
    }

    return num_tags;
}


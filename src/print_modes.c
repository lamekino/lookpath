#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> /* NAME_MAX */

#include "print_modes.h"
#include "arguments.h"
#include "debug_assert.h"

static int compare(const void *a, const void *b) {
    // return strncmp(*((const char **) a), *((const char **) b), NAME_MAX);
    return strcmp(*((const char **) a), *((const char **) b));
}

static void print_as_sorted(const tagged_list *tags) {
    qsort(tags->strings, tags->num_strings, sizeof(char *), &compare);
    for (size_t i = 0; i < tags->num_strings; i++) {
        puts(tags->strings[i]);
    }
}

static void print_as_full_path(const tagged_list *tags) {
    struct tag *it;
    for (it = tags->tags; it <= &tags->tags[tags->num_tags - 1]; it++) {
        /* when no matches in tag, length == position */
        for (size_t idx = it->position; idx < it->length; idx++) {
            printf("%s/%s\n", it->name, tags->strings[idx]);
        }
    }
}

static void print_as_sorted_full_path(const tagged_list *tags) {
    struct tag *it;
    for (it = tags->tags; it <= &tags->tags[tags->num_tags - 1]; it++) {
        qsort(&tags->strings[it->position],
                it->length - it->position,
                sizeof(char *),
                &compare);

        for (size_t idx = it->position; idx < it->length; idx++) {
            printf("%s/%s\n", it->name, tags->strings[idx]);
        }
    }
}

static void print_as_tree(const tagged_list *tags) {
    /* TODO: make this use wchar... */
    const char *POINT = "├─";
    const char *LAST_POINT = "└─";

    struct tag *it;
    for (it = tags->tags; it <= &tags->tags[tags->num_tags - 1]; it++) {
        if (!has_matches(it)) {
            continue;
        }

        printf("%s:\n", it->name);
        for (size_t idx = it->position; idx < it->length; idx++) {
            const char *point = POINT;

            if (idx == it->length - 1) {
                point = LAST_POINT; /* why does this work??? */
            }
            printf("%s %s\n", point, tags->strings[idx]);
        }
    }
}

static void print_as_sorted_tree(const tagged_list *tags) {
    /* TODO: make this use wchar... */
    const char *POINT = "├─";
    const char *LAST_POINT = "└─";

    struct tag *it;
    for (it = tags->tags; it <= &tags->tags[tags->num_tags - 1]; it++) {
        if (!has_matches(it)) {
            continue;
        }

        qsort(&tags->strings[it->position],
              it->length - it->position,
              sizeof(char *),
              &compare);

        printf("%s:\n", it->name);
        for (size_t idx = it->position; idx < it->length; idx++) {
            const char *point = POINT;

            if (idx == it->length - 1) {
                point = LAST_POINT; /* why does this work??? */
            }
            printf("%s %s\n", point, tags->strings[idx]);
        }
    }
}

printer get_print_mode(enum print_mode pm) {
    printer printers[] = {
        [PM_SORTED] = &print_as_sorted,
        [PM_FULL_PATH] = &print_as_full_path,
        [PM_FULL_PATH_SORTED] = &print_as_sorted_full_path,
        [PM_TREE] = &print_as_tree,
        [PM_TREE_SORTED] = &print_as_sorted_tree,
    };

    STATIC_ASSERT(
        sizeof(printers)/sizeof(printers[0])
            == END_PRINT_MODE - START_PRINT_MODE,
        missing_print_mode_impl
    );

    ASSERT(0 < pm && pm < END_PRINT_MODE);

    return NULL;
}

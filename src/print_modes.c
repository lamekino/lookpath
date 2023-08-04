#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <wchar.h>

#include "bits.h"
#include "print_modes.h"
#include "arguments.h"
#include "debug_assert.h"
#include "settings.h"

static int compare(const void *a, const void *b) {
    return strncmp(*((const char **) a), *((const char **) b), NAME_MAX);
}

static void print_as_sorted(const tagged_list *tags) {
    qsort(tags->strings, tags->num_strings, sizeof(char *), &compare);
    for (size_t i = 0; i < tags->num_strings; i++) {
        puts(tags->strings[i]);
    }
}

static void print_as_full_path(const tagged_list *tags) {
    struct tag *it;
    for (it = tags->tags; it < &tags->tags[tags->num_tags]; it++) {
        /* when no matches in tag, length == position */
        for (size_t idx = it->position; idx < it->length; idx++) {
            printf("%s/%s\n", it->name, tags->strings[idx]);
        }
    }
}

static void print_as_sorted_full_path(const tagged_list *tags) {
    struct tag *it;
    for (it = tags->tags; it < &tags->tags[tags->num_tags]; it++) {
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
    const wchar_t *POINT = L"├─";
    const wchar_t *LAST_POINT = L"└─";

    struct tag *it;

    fwide(stdout, 1);
    for (it = tags->tags; it < &tags->tags[tags->num_tags]; it++) {
        if (!has_matches(it)) {
            continue;
        }

        wprintf(L"%s:\n", it->name);
        for (size_t idx = it->position; idx < it->length; idx++) {
            const wchar_t *point = POINT;

            if (idx == it->length - 1) {
                point = LAST_POINT; /* why does this work??? */
            }
            wprintf(L"%ls %s\n", point, tags->strings[idx]);
        }
    }
    fwide(stdout, -1);
}

static void print_as_sorted_tree(const tagged_list *tags) {
    const wchar_t *POINT = L"├─";
    const wchar_t *LAST_POINT = L"└─";

    struct tag *it;

    fwide(stdout, 1);
    for (it = tags->tags; it < &tags->tags[tags->num_tags]; it++) {
        if (!has_matches(it)) {
            continue;
        }

        qsort(&tags->strings[it->position],
              it->length - it->position,
              sizeof(char *),
              &compare);

        wprintf(L"%s:\n", it->name);
        for (size_t idx = it->position; idx < it->length; idx++) {
            const wchar_t *point = POINT;

            if (idx == it->length - 1) {
                point = LAST_POINT; /* why does this work??? */
            }
            wprintf(L"%ls %s\n", point, tags->strings[idx]);
        }
    }
    fwide(stdout, -1);
}

print_mode_fp get_print_mode(const packed_t pmask) {
    const print_mode_fp printers[] = {
        [PM_SORTED] = &print_as_sorted,
        [PM_FULL_PATH] = &print_as_full_path,
        [PM_FULL_PATH_SORTED] = &print_as_sorted_full_path,
        [PM_TREE] = &print_as_tree,
        [PM_TREE_SORTED] = &print_as_sorted_tree,
    };

    STATIC_ASSERT(IS_LENGTH(NUM_PRINT_MODES, printers),
        missing_print_mode_impl);

    switch (pmask) {
        case EMPTY_MASK: return printers[0];
#define CASE(label, ...) case MASK(PM_##label): return printers[PM_##label];
        PRINT_FLAGS(CASE)
#undef CASE
        default: break;
    }
    fprintf(stderr, "multiple flags entered, using -S mode\n");
    return &print_as_sorted;
}

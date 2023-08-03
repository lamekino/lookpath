#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdbool.h>

#include "debug_assert.h"

enum category {
    PRINT_FLAGS,
#define PRINT_FLAGS(X) \
    X(SORTED, "-S", \
        "print as sorted list (default)") \
    X(FULL_PATH, "-f", \
        "print as full path, unsorted") \
    X(FULL_PATH_SORTED, "-F", \
        "print as full path, sorted by file basename") \
    X(TREE, "-t", \
        "print as unsorted tree") \
    X(TREE_SORTED, "-T", \
        "print as tree sorted by file basename")

    QUERY_FLAGS,
#define QUERY_FLAGS(X) \
    X(LEFT_TO_RIGHT, "-L", \
        "search from left to right (default)") \
    X(RIGHT_TO_LEFT, "-r", \
        "search from right to left") \
    X(CASE_INSENSITIVE, "-i", \
        "ignore filename case")

    MISC_FLAGS,
#define MISC_FLAGS(X) \
    X(HELP, "-h", \
        "show this help")

    NUM_CATEGORIES
};

#define ALL_FLAGS(X) \
    PRINT_FLAGS(X) \
    QUERY_FLAGS(X) \
    MISC_FLAGS(X)

#define COUNT_FLAGS(...) +1

enum argument {
#define FLAG_ENUM(label, ...) FLAG_##label,
#define BOUNDED_ENUM(xs) START_##xs, xs(FLAG_ENUM) END_##xs
    BOUNDED_ENUM(PRINT_FLAGS),
    BOUNDED_ENUM(QUERY_FLAGS),
    BOUNDED_ENUM(MISC_FLAGS),
#undef FLAG_ENUM
#undef BOUNDED_ENUM
    ARGUEMENTS_LEN
};

#define NUM_FLAGS ALL_FLAGS(COUNT_FLAGS)
#define NUM_ARGUMENTS (ARGUEMENTS_LEN - 2 * NUM_CATEGORIES)

/* ensure that the number of bounded argument equals the number of defined flags
 * at compile time */
STATIC_ASSERT(NUM_ARGUMENTS == NUM_FLAGS,
        missing_argument_in_bounded_enum);

#undef NUM_FLAGS

#define IS_LENGTH(bound, arr) \
    (sizeof((arr))/sizeof(*(arr)) == bound)

const char *get_flag_string(enum argument argument);
const char *get_description(enum argument argument);
#if 0
/* TODO: */
const char *get_flag_full(enum argument argument); /* --flag-name */
const char *get_flag_terse(enum argument argument); /* -f */
#endif

enum argument get_start(enum category catagory);
enum argument get_end(enum category catagory);
enum argument get_first(enum category category);
enum argument get_last(enum category category);

int get_base_enum(enum argument bounded);

bool is_category_member(enum argument a, enum category c);
bool is_category(int c);
#endif

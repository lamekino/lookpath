#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include "debug_assert.h"
#include <stdbool.h>

enum catagory {
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
    X(REVERSE_SEARCH, "-r", \
        "search from right to left instead of left to right")

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
#define NUM_FLAGS ALL_FLAGS(COUNT_FLAGS)

enum arguments {
#define FLAG_ENUM(label, ...) FLAG_##label,
#define BOUNDED_ENUM(xs) START_##xs, xs(FLAG_ENUM) END_##xs
    BOUNDED_ENUM(PRINT_FLAGS),
    BOUNDED_ENUM(QUERY_FLAGS),
    BOUNDED_ENUM(MISC_FLAGS),
#undef FLAG_ENUM
#undef BOUNDED_ENUM
    ARGUEMENTS_LEN
};

#define NUM_ARGUMENTS (ARGUEMENTS_LEN - 2 * NUM_CATEGORIES)

/* ensure that the number of bounded argument equals the number of defined flags
 * at compile time */
STATIC_ASSERT(NUM_ARGUMENTS == NUM_FLAGS,
        missing_argument_in_bounded_enum);

const char *get_argument(enum arguments argument);
const char *get_description(enum arguments argument);

enum arguments get_start(enum catagory catagory);
enum arguments get_end(enum catagory catagory);

bool is_print_arg(enum arguments a);
bool is_query_arg(enum arguments a);
bool is_misc_arg(enum arguments a);
bool is_catagory_member(enum arguments a, enum catagory c);
bool is_catagory(int c);
#endif

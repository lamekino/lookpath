#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdbool.h>

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

#define MISC_FLAGS(X) \
    X(HELP, "-h", \
        "show this help")

#define ALL_FLAGS(X) \
    PRINT_FLAGS(X) \
    MISC_FLAGS(X)

enum arguments {
#define FLAG_ENUM(label, ...) FLAG_##label,
    START_PRINT_FLAGS,
    PRINT_FLAGS(FLAG_ENUM)
    END_PRINT_FLAGS,

    START_MISC_FLAGS,
    MISC_FLAGS(FLAG_ENUM)
    END_MISC_FLAGS,
#undef FLAG_ENUM
    LEN_ARGUMENTS
};

enum print_mode {
    START_PRINT_MODE = START_PRINT_FLAGS,
#define PRINT_MODE_ENUM(label, ...) PM_##label,
    PRINT_FLAGS(PRINT_MODE_ENUM)
#undef PRINT_MODE_ENUM
    END_PRINT_MODE
};

const char *get_argument(enum arguments argument);
const char *get_description(enum arguments argument);
bool is_print_arg(enum arguments a);
bool is_misc_arg(enum arguments a);
#endif

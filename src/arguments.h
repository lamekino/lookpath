#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdbool.h>

#define PRINT_FLAGS(X) \
    X(SORTED, "-S") \
    X(FULL_PATH, "-f") \
    X(FULL_PATH_SORTED, "-F") \
    X(TREE, "-t") \
    X(TREE_SORTED, "-T")

#define MISC_FLAGS(X) \
    X(HELP, "-h")

#define ALL_FLAGS(X) \
    PRINT_FLAGS(X) \
    MISC_FLAGS(X)

enum arguments {
#define FLAG_ENUM(label, _) FLAG_##label,
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
#define PRINT_MODE_ENUM(label, _) PM_##label,
    PRINT_FLAGS(PRINT_MODE_ENUM)
#undef PRINT_MODE_ENUM
    END_PRINT_MODE
};

const char *get_argument(enum arguments argument);
const char *get_description(enum arguments argument);
bool is_print_arg(enum arguments a);
bool is_misc_arg(enum arguments a);
#endif

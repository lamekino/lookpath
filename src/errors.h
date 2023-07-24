#ifndef ERRORS_H
#define ERRORS_H
#include <limits.h>

enum error {
    FAILED_BUFFER_ALLOC = INT_MIN,
    FAILED_BUFFER_REALLOC,
    NO_PATH_VAR,
    EMPTY_PATH_VAR,
    INSUFFICIENT_ARGS,
    WRONG_ARGS,
    DOUBLE_PATTERN,
    MISSING_PATTERN,
    SHOW_USAGE,

    NO_ERROR = 0
};
#define HAS_ERROR(p) ((p) < NO_ERROR)

#endif

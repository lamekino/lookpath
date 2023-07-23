#ifndef SETTINGS_H
#define SETTINGS_H
#include <stddef.h>
#include <stdbool.h>

#include "arguments.h"

enum order {
    IN_PLACE,
    REVERSE,
    LEN_ORDER
};

enum print_mode {
#define PRINT_MODE_ENUM(label, ...) PM_##label,
    PRINT_FLAGS(PRINT_MODE_ENUM)
#undef PRINT_MODE_ENUM
    LEN_PRINT_MODES
};

typedef struct {
    enum print_mode print_mode;
    enum order order;
    char *pattern;
    size_t pattern_len;
} settings_t;

#endif

#include <stdbool.h>

#include "arguments.h"
#include "debug_assert.h"

const char *get_argument(enum arguments argument) {
    const char *flags[] = {
#define FLAG_STRINGS(label, flag, _) [FLAG_##label] = flag,
    ALL_FLAGS(FLAG_STRINGS)
#undef FLAG_STRINGS
    };

    ASSERT(0 < argument && argument < LEN_ARGUMENTS);

    /* cursed ;) */
    return argument[flags];
}

const char *get_description(enum arguments argument) {
    const char *descriptions[] = {
#define FLAG_STRINGS(label, _, desc) [FLAG_##label] = desc,
    ALL_FLAGS(FLAG_STRINGS)
#undef FLAG_STRINGS
    };

    ASSERT(0 < argument && argument < LEN_ARGUMENTS);

    return argument[descriptions];
}

bool is_print_arg(enum arguments a) {
    return START_PRINT_FLAGS < a && a < END_PRINT_FLAGS;
}

bool is_misc_arg(enum arguments a) {
    return START_MISC_FLAGS < a && a < END_MISC_FLAGS;
}

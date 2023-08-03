#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "arguments.h"
#include "debug_assert.h"
#include "errors.h"
#include "parse_arguments.h"
#include "print_modes.h"
#include "search_methods.h"
#include "settings.h"
#include "usage.h"

#define FLAG_CAP 2

static bool set_pattern(settings_t *settings, char *given) {
    if (given[0] == '-') {
        return false;
    }

    settings->pattern = given;
    settings->pattern_len = strlen(given);
    return true;
}

#if 0
typedef int *(*get_field_fp)(const settings_t *, enum argument);

static int *get_print_field(const settings_t *settings, enum argument a) {
    (void) a;
    return (int *) settings->print_mode;
}

static int *get_query_field(const settings_t *settings, enum argument a) {
    if (a == FLAG_CASE_INSENSITIVE) return NULL;
    return (int *) settings->strategy;
}

static int *get_misc_field(const settings_t *settings, enum argument a) {
    (void) settings;
    (void) a;
    return NULL;
}
#endif

static int *get_settings_field(const settings_t *settings,
                               enum argument a) {
#define F(label) FLAG_##label

#define CASE(label, block) \
    do { \
        case F(label): block \
    } while (0);

#define CASE_PRINT(label, ...) \
    CASE(label, { return (int *) settings->print_mode; })

#define CASE_QUERY(label, ...) \
    CASE(label, { return NULL; })

#define CASE_MISC(label, ...) \
    CASE(label, { return NULL; })

    /* TODO: convert to jump table and assert length at compile time */
    switch (a) {
        PRINT_FLAGS(CASE_PRINT)
        QUERY_FLAGS(CASE_QUERY)
        MISC_FLAGS(CASE_MISC)
        default: break;
    }

#undef CASE_PRINT
#undef CASE_QUERY
#undef CASE_MISC
#undef CASE
#undef F

    ASSERT(0 && "unreachable");
    return NULL;
}

static enum argument set_option(settings_t *settings,
                                enum category category,
                                const char *given) {
    const enum argument end = get_end(category);
    enum argument it = get_start(category);

    while (++it < end) {
        const char *flag_string = get_flag_string(it);

        if (strncmp(given, flag_string, FLAG_CAP) == 0) {
            int *settings_field = get_settings_field(settings, it);

            if (settings_field != NULL) {
                *settings_field = get_base_enum(it);
            }

            settings->mask[category] |= MASK(get_base_enum(it));
            break;
        }
    }

    /* if no matches are found, then the iterator will be set to END_*, meaning
     * is_category_member() will return false */
    return it;
}

enum error parse_arguments(settings_t *settings, int argc, char **argv) {
    for (int idx = 1; idx < argc; idx++) {
        bool is_flag;
        const char *old_pattern = settings->pattern;

        if (set_pattern(settings, argv[idx])) {
            if (old_pattern != settings->pattern) continue;
            return DOUBLE_PATTERN;
        }

        for (enum category c = 0; c < NUM_CATEGORIES; c++) {
            enum argument arg = set_option(settings, c, argv[idx]);

            is_flag = is_category_member(arg, c);

            if (!is_flag) continue;
            if (arg == FLAG_HELP) return SHOW_USAGE;

            break;
        }

        if (!is_flag) return WRONG_ARGS;
    }

    return settings->pattern == NULL
        ? MISSING_PATTERN
        : NO_ERROR;
}

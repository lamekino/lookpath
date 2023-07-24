#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "arguments.h"
#include "debug_assert.h"
#include "errors.h"
#include "parse_arguments.h"
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

static int *get_settings_field(const settings_t *settings,
                               enum category c) {
    int *fields[] = {
        [PRINT_FLAGS] = (int *) &settings->print_mode,
        [QUERY_FLAGS] = (int *) &settings->strategy,
        [MISC_FLAGS] = NULL
    };
    STATIC_ASSERT(IS_LENGTH(NUM_CATEGORIES, fields), missing_settings_field);

    return fields[c];
}

static enum argument set_option(settings_t *settings,
                                enum category cat,
                                const char *given) {
    const enum argument end = get_end(cat);
    enum argument it = get_start(cat);

    while (++it < end) {
        const char *flag_string = get_flag_string(it);
        bool found_argument = strncmp(given, flag_string, FLAG_CAP) == 0;
        int *settings_field = get_settings_field(settings, cat);

        if (found_argument) {
            if (settings_field != NULL) {
                *settings_field = (int) get_base_enum(it);
            }
            break;
        }
    }

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

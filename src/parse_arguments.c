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

typedef void (*change_settings_fp)(settings_t *, enum argument);

static bool set_pattern(settings_t *settings, char *given) {
    if (given[0] == '-') {
        return false;
    }

    settings->pattern = given;
    settings->pattern_len = strlen(given);
    return true;
}

static enum argument set_option(settings_t *settings,
                                const char *given,
                                change_settings_fp apply,
                                enum argument start,
                                enum argument end) {
    enum argument it = start;

    while (++it < end) {
        const char *flag_string = get_flag_string(it);
        bool found_argument = strncmp(given, flag_string, FLAG_CAP) == 0;

        if (found_argument) {
            apply(settings, it);
            break;
        }
    }

    return it;
}

static void apply_print_mode(settings_t *settings, enum argument arg) {
    settings->print_mode = (enum print_mode) (arg - START_PRINT_FLAGS - 1);
}

static void apply_misc_option(settings_t *settings, enum argument arg) {
    (void) settings;
    (void) arg;
    /* do nothing */
}

static void apply_query_option(settings_t *settings, enum argument arg) {
    settings->strategy = (enum search_methods) (arg - START_QUERY_FLAGS);
}

static change_settings_fp get_setter(enum catagory catagory) {
    ASSERT(is_catagory(catagory));

    static const change_settings_fp setters[] = {
        [PRINT_FLAGS] = &apply_print_mode,
        [QUERY_FLAGS] = &apply_query_option,
        [MISC_FLAGS] = &apply_misc_option
    };
    STATIC_ASSERT(IS_LENGTH(NUM_CATEGORIES, setters), missing_parsing_impl);

    return setters[catagory];
}


enum error parse_arguments(settings_t *settings, int argc, char **argv) {
    for (int idx = 1; idx < argc; idx++) {
        bool is_flag;
        const char *old_pattern = settings->pattern;

        if (set_pattern(settings, argv[idx])) {
            if (old_pattern != settings->pattern) continue;
            return DOUBLE_PATTERN;
        }

        for (enum catagory c = 0; c < NUM_CATEGORIES; c++) {
            enum argument arg = set_option(settings, argv[idx], get_setter(c),
                                            get_start(c), get_end(c));

            is_flag = is_catagory_member(arg, c);

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

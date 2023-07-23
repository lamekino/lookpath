#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "arguments.h"
#include "debug_assert.h"
#include "errors.h"
#include "parse_arguments.h"
#include "settings.h"
#include "usage.h"

#define FLAG_CAP 2

typedef void (*apply_f)(settings_t *, enum arguments);

static bool set_pattern(settings_t *settings, char *given) {
    if (given[0] == '-') {
        return false;
    }

    settings->pattern = given;
    settings->pattern_len = strlen(given);
    return true;
}

static enum arguments set_option(settings_t *settings,
                                 const char *given,
                                 apply_f apply,
                                 enum arguments start,
                                 enum arguments end) {
    enum arguments it = start;

    while (++it < end) {
        if (strncmp(given, get_argument((enum arguments) it), FLAG_CAP) == 0) {
            apply(settings, it);
            break;
        }
    }

    return it;
}

static void apply_print_mode(settings_t *settings, enum arguments arg) {
    settings->print_mode = (enum print_mode) arg - START_PRINT_FLAGS - 1;
}

static void apply_misc_option(settings_t *settings, enum arguments arg) {
    (void) settings;
    (void) arg;
    /* do nothing */
}

static void apply_query_option(settings_t *settings, enum arguments arg) {
    settings->order = (enum order) arg - START_QUERY_FLAGS;
}

static enum arguments set_print_mode(settings_t *settings, char *given) {
    return set_option(settings, given, &apply_print_mode,
            START_PRINT_FLAGS, END_PRINT_FLAGS);
}

static enum arguments set_misc_option(settings_t *settings, char *given) {
    return set_option(settings, given, &apply_misc_option,
            START_MISC_FLAGS, END_MISC_FLAGS);
}

static enum arguments set_query_mode(settings_t *settings, char *given) {
    return set_option(settings, given, &apply_query_option,
            START_QUERY_FLAGS, END_QUERY_FLAGS);
}

int parse_arguments(settings_t *settings, int argc, char **argv) {
    for (int idx = 1; idx < argc; idx++) {
        enum arguments arg;
        const char *old_pattern = settings->pattern;

        if (set_pattern(settings, argv[idx])) continue;

        /* TODO: generalize this? */
        arg = set_print_mode(settings, argv[idx]);
        if (is_print_arg(arg)) continue;

        arg = set_query_mode(settings, argv[idx]);
        if (is_query_arg(arg)) continue;

        arg = set_misc_option(settings, argv[idx]);
        if (arg == FLAG_HELP) return SHOW_USAGE;
        if (is_misc_arg(arg)) continue;

        if (settings->pattern != NULL && settings->pattern != old_pattern) {
            return DOUBLE_PATTERN;
        }

        return WRONG_ARGS;
    }

    if (settings->pattern == NULL) {
        return NO_PATTERN;
    }

    return NO_ERROR;
}

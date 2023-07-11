#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "arguments.h"
#include "errors.h"
#include "parse_arguments.h"
#include "settings.h"
#include "usage.h"

#define FLAG_CAP 2

static bool set_pattern(settings_t *settings, char *given) {
    if (given[0] == '-') {
        return false;
    }

    settings->pattern = given;
    return true;
}

static enum arguments set_print_mode(settings_t *settings, const char *given) {
    enum print_mode it = START_PRINT_MODE;

    while (++it < END_PRINT_MODE) {
        if (strncmp(given, get_argument((enum arguments) it), FLAG_CAP) == 0) {
            settings->print_mode = it;
            break;
        }
    }

    return (enum arguments) it;
}

static enum arguments set_misc(settings_t *settings, char *given) {
    (void) settings;
    enum arguments it = START_MISC_FLAGS;

    while (++it < END_MISC_FLAGS) {
        if (strncmp(given, get_argument((enum arguments) it), FLAG_CAP) == 0) {
            break;
        }
    }

    return (enum arguments) it;
}

int parse_arguments(settings_t *settings, int argc, char **argv) {
    for (int idx = 1; idx < argc; idx++) {
        enum arguments arg;
        const char *old_pattern = settings->pattern;

        if (set_pattern(settings, argv[idx])) {
            continue;
        }

        arg = set_print_mode(settings, argv[idx]);
        if (is_print_arg(arg)) {
            continue;
        }

        arg = set_misc(settings, argv[idx]);
        if (arg == FLAG_HELP) {
            return SHOW_USAGE;
        }
        if (is_misc_arg(arg)) {
            continue;
        }

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

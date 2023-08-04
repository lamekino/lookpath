#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "arguments.h"
#include "bits.h"
#include "debug_assert.h"
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
    settings->pattern_len = strlen(given);
    return true;
}

#if 0
static enum argument set_option(settings_t *settings,
                                enum category category,
                                const char *given) {
    const enum argument end = get_end(category);

    for (enum argument it = get_first(category); it < end; it++) {
        const char *flag_string = get_flag_string(it);

        if (strncmp(given, flag_string, FLAG_CAP) == 0) {
            PACK_MASK(settings->mask[category], get_base_enum(it));
            return it;
        }
    }

    return end;
}
#endif

enum error parse_arguments(settings_t *settings, int argc, char **argv) {
    for (int idx = 1; idx < argc; idx++) {
        const char *old_pattern = settings->pattern;

        if (set_pattern(settings, argv[idx])) {
            if (old_pattern != NULL && old_pattern != settings->pattern) {
                return DOUBLE_PATTERN;
            }
            continue;
        }

        enum argument id = validate_arg(argv[idx]);
        if (is_bounded(id)) {
            if (id == FLAG_HELP) return SHOW_USAGE;

            PACK_MASK(settings->mask[get_catagory(id)], get_base_enum(id));
            continue;
        }

        return WRONG_ARGS;
    }

    return settings->pattern == NULL
        ? MISSING_PATTERN
        : NO_ERROR;
}

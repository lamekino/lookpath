#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>
#include <limits.h>
#include <dirent.h>

#include "errors.h"
#include "print_modes.h"
#include "settings.h"
#include "usage.h"
#include "parse_arguments.h"
#include "tag_matches.h"
#include "do_settings.h"
#include "what_happened.h"

int main(int argc, char *argv[]) {
    /* hold's the user's settings determined by program flags,
     * parse_arguments() */
    settings_t settings = {0};

    /* points to the $PATH environment variable */
    char *env_path = NULL;

    /* stores the result for error functions */
    enum error err = NO_ERROR;

    /*
     * tags.tags: the pointer to the list of tags
     * tags.num_tags: the number of tags in tags.tags
     * tags.strings: the pointer to the tagged list
     * tags.num_strings: the number of strings
     */
    tagged_list tags = {0};

    setlocale(LC_CTYPE, "");

    if (argc < 2) {
        return what_happened(INSUFFICIENT_ARGS, argv[0]);
    }

    env_path = getenv("PATH");
    if (env_path == NULL) {
        return what_happened(NO_PATH_VAR, argv[0]);
    }
    if (strlen(env_path) == 0) {
        return what_happened(EMPTY_PATH_VAR, argv[0]);
    }

    err = parse_arguments(&settings, argc, argv);
    if (HAS_ERROR(err)) {
        return what_happened(err, argv[0]);
    }

    err = tag_matches(env_path, settings.pattern, &tags);
    if (!HAS_ERROR(err)) {
        err = do_settings(&settings, &tags);
    }

    clear_tags(&tags);
    return what_happened(err, argv[0]);
}

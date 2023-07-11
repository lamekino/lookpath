#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

#include "errors.h"
#include "print_modes.h"
#include "settings.h"
#include "usage.h"
#include "parse_arguments.h"
#include "tag_matches.h"

int what_happened(enum error code, const char *prog_name) {
    (void) prog_name;
    switch (code) {
    case FAILED_BUFFER_ALLOC:
        perror("malloc");
        fprintf(stderr, "failed to allocate buffer\n");
        break;
    case FAILED_BUFFER_REALLOC:
        perror("realloc");
        fprintf(stderr, "failed to reallocate buffer\n");
        break;
    case NO_PATH_VAR:
        fprintf(stderr, "PATH variable not set, quitting\n");
        break;
    case EMPTY_PATH_VAR:
        fprintf(stderr, "PATH is empty string, qutting\n");
        break;
    case INSUFFICIENT_ARGS:
        fprintf(stderr, "requires a 1 argument [pattern]\n");
        break;
    case WRONG_ARGS:
        fprintf(stderr, "mismatched or invalid argument provided, quitting\n");
        break;
    case DOUBLE_PATTERN:
        fprintf(stderr, "can only take one pattern, quitting\n");
        break;
    case NO_PATTERN:
        fprintf(stderr, "no pattern provided in arguments\n");
        break;
    case SHOW_USAGE:
        usage(prog_name);
        return EXIT_SUCCESS;
    default:
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

int do_settings(const settings_t *settings, tagged_list *tags) {
    get_print_mode(settings->print_mode);
    return NO_ERROR;
}

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

    err = tag_matches (env_path, settings.pattern, &tags);
    if (!HAS_ERROR(err)) {
        err = do_settings(&settings, &tags);
    }

    clear_tags(&tags);
    return what_happened(err, argv[0]);
}

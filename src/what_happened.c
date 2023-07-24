#include <stdio.h>
#include <stdlib.h>

#include "what_happened.h"
#include "usage.h"
#include "errors.h"

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
    case MISSING_PATTERN:
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


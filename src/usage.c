#include <stdio.h>

#include "parse_arguments.h"
#include "usage.h"
#include "arguments.h"

void usage(const char *prog_name) {
    enum arguments it;

    printf("%s:\n", prog_name);

    printf("  display options:\n");
    it = START_PRINT_FLAGS;
    while (++it < END_PRINT_FLAGS) {
        printf("    %s:  %s\n", get_argument(it), get_description(it));
    }

    printf("  miscellaneous:\n");
    it = START_MISC_FLAGS;
    while (++it < END_MISC_FLAGS) {
        printf("    %s:  %s\n", get_argument(it), get_description(it));
    }
}

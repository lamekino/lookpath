#include <stdio.h>

#include "parse_arguments.h"
#include "usage.h"
#include "arguments.h"

static void list_flags(const char *title,
                       enum arguments start,
                       enum arguments end) {
    enum arguments it = start;
    printf("  %s:\n", title);
    while (++it < end) {
        printf("    %s:  %s\n", get_argument(it), get_description(it));
    }

}

void usage(const char *prog_name) {
    enum arguments it;

    printf("%s:\n", prog_name);

    list_flags("display options", START_PRINT_FLAGS, END_PRINT_FLAGS);
    list_flags("query options", START_QUERY_FLAGS, END_QUERY_FLAGS);
    list_flags("miscellaneous", START_MISC_FLAGS, END_MISC_FLAGS);
}

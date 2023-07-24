#include <stdio.h>

#include "debug_assert.h"
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
    printf("%s:\n", prog_name);

    const char *titles[] = {
        [PRINT_FLAGS] = "display options",
        [QUERY_FLAGS] = "query options",
        [MISC_FLAGS] = "miscellaneous"
    };
    STATIC_ASSERT(IS_LENGTH(NUM_CATEGORIES, titles), missing_usage_title);

    for (enum catagory c = 0; c < NUM_CATEGORIES; c++) {
        list_flags(titles[c], get_start(c), get_end(c));
    }
}

#include <stdbool.h>

#include "arguments.h"
#include "debug_assert.h"

#ifdef DEBUG
void assert_catagory(enum argument arg) {
    bool is_member = false;
    for (enum category c = 0; c < NUM_CATEGORIES; c++) {
        is_member |= is_category_member(arg, c);
    }

    assert(is_member);
}
#define ASSERT_IN_CATEGORY(a) assert_catagory((a))
#else
#define ASSERT_IN_CATEGORY(_)
#endif

const char *get_flag_string(enum argument argument) {
    ASSERT_IN_CATEGORY(argument);

    const char *flags[] = {
#define FLAG_STRINGS(label, flag, _) [FLAG_##label] = flag,
    ALL_FLAGS(FLAG_STRINGS)
#undef FLAG_STRINGS
    };

    /* cursed ;) */
    return argument[flags];
}

const char *get_description(enum argument argument) {
    ASSERT_IN_CATEGORY(argument);

    const char *descriptions[] = {
#define FLAG_STRINGS(label, _, desc) [FLAG_##label] = desc,
    ALL_FLAGS(FLAG_STRINGS)
#undef FLAG_STRINGS
    };

    return argument[descriptions];
}

enum argument get_start(enum category catagory) {
    ASSERT(is_category(catagory));

    const enum argument start_values[] = {
        [PRINT_FLAGS] = START_PRINT_FLAGS,
        [QUERY_FLAGS] = START_QUERY_FLAGS,
        [MISC_FLAGS] = START_MISC_FLAGS
    };
    STATIC_ASSERT(IS_LENGTH(NUM_CATEGORIES, start_values),
            missing_get_start_value);

    return start_values[catagory];
}

enum argument get_end(enum category catagory) {
    ASSERT(is_category(catagory));

    const enum argument end_values[] = {
        [PRINT_FLAGS] = END_PRINT_FLAGS,
        [QUERY_FLAGS] = END_QUERY_FLAGS,
        [MISC_FLAGS] = END_MISC_FLAGS
    };
    STATIC_ASSERT(IS_LENGTH(NUM_CATEGORIES, end_values), missing_get_end_value);

    return end_values[catagory];
}

enum argument get_first(enum category category) {
    return get_start(category) + 1;
}

enum argument get_last(enum category category) {
    return get_end(category) - 1;
}

bool is_category_member(enum argument a, enum category c) {
    return get_start(c) < a && a < get_end(c);
}

bool is_category(int c) {
    return 0 <= c && c < NUM_CATEGORIES;
}

static enum category get_catagory(enum argument a) {
    for (enum category c = 0; c < NUM_CATEGORIES; c++) {
        if (is_category_member(a, c)) {
            return c;
        }
    }

    ASSERT(0 && "unreachable");
    return -1;
}

int get_base_enum(enum argument bounded) {
    int start = get_start(get_catagory(bounded));

    return start == 0
        ? bounded - 1
        : bounded - start - 1;
}

#include <check.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/tagged_list.h"

START_TEST(test_clear_tags_empty) {
    tagged_list tags = {0};

    clear_tags(&tags);

    ck_assert_ptr_null(tags.tags);
    ck_assert_ptr_null(tags.strings);

    ck_assert_uint_eq(tags.num_tags, 0u);
    ck_assert_uint_eq(tags.num_strings, 0u);
} END_TEST

START_TEST(test_clear_tags_singleton) {
    tagged_list tags = {0};
    add_tag(&tags, "singleton", 0, 0);

    clear_tags(&tags);

    ck_assert_ptr_null(tags.tags);
    ck_assert_ptr_null(tags.strings);

    ck_assert_uint_eq(tags.num_tags, 0u);
    ck_assert_uint_eq(tags.num_strings, 0u);
} END_TEST

START_TEST(test_clear_tags_multiple) {
    tagged_list tags = {0};
    add_tag(&tags, "test a", 0, 0);
    add_tag(&tags, "test b", 666, 1337);
    add_tag(&tags, "test c", 69, 420);

    clear_tags(&tags);

    ck_assert_ptr_null(tags.tags);
    ck_assert_ptr_null(tags.strings);

    ck_assert_uint_eq(tags.num_tags, 0u);
    ck_assert_uint_eq(tags.num_strings, 0u);
} END_TEST

Suite *usage_suite() {
    Suite *suite;
    TCase *tc_core;

    suite = suite_create("tagged_list");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_clear_tags_empty);
    tcase_add_test(tc_core, test_clear_tags_singleton);
    tcase_add_test(tc_core, test_clear_tags_multiple);
    suite_add_tcase(suite, tc_core);

    return suite;
}

int main() {
    int failed;

    Suite *suite = usage_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);

    srunner_free(runner);
    return failed > 0;
}

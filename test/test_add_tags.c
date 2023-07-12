#include <check.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/tagged_list.h"

START_TEST(test_add_tags_basic) {
    tagged_list tags = {0};

    add_tag(&tags, "test", 0, 0);
    ck_assert_ptr_nonnull(tags.tags);
    ck_assert_uint_eq(tags.num_tags, 1u);

    clear_tags(&tags);
} END_TEST

START_TEST(test_add_tags_multiple) {
    tagged_list tags = {0};

    for (size_t i = 0; i < 10; i++) {
        char name[1] = { i + '0' } ;
        add_tag(&tags, name, i, 0);

        ck_assert_ptr_nonnull(tags.tags);
        ck_assert_uint_eq(tags.num_tags, i + 1);
        ck_assert_str_eq(tags.tags[tags.num_tags - 1].name, name);
    }

    clear_tags(&tags);
} END_TEST

START_TEST(test_add_tags_conflict) {
    tagged_list tags = {0};

    for (size_t i = 0; i < 10; i++) {
        add_tag(&tags, "test", i, 0);

        ck_assert_ptr_nonnull(tags.tags);
        ck_assert_uint_eq(tags.num_tags, i + 1);
        ck_assert_str_eq(tags.tags[tags.num_tags - 1].name, "test");
    }

    clear_tags(&tags);
} END_TEST

Suite *create_suite() {
    Suite *suite = suite_create(__FILE__);
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_add_tags_basic);
    tcase_add_test(tc_core, test_add_tags_multiple);
    tcase_add_test(tc_core, test_add_tags_conflict);
    suite_add_tcase(suite, tc_core);

    return suite;
}

int main() {
    int failed;

    Suite *suite = create_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);

    srunner_free(runner);
    return failed > 0;
}

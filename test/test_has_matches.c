#include <check.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/tagged_list.h"

START_TEST(test_has_matches_no_match) {
    tagged_list tags = {0};

    clear_tags(&tags);
} END_TEST

Suite *create_suite() {
    Suite *suite = suite_create(__FILE__);
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_has_matches_no_match);
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

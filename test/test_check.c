#include <check.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/usage.h"

START_TEST(test_usage_basic) {
    char *program_name = __FILE__;

    usage(program_name);
    ck_assert_str_eq(program_name, __FILE__);
} END_TEST

Suite *usage_suite() {
    Suite *suite;
    TCase *tc_core;

    suite = suite_create("usage");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, NULL);
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

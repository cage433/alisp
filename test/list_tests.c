#include "list.h"
#include "check.h"
#include "test_suites.h"

START_TEST(test_list_length)
{
    List *l = NULL;
    fail_unless(listlen(l) == 0);
    l = cons("foo", l);
    fail_unless(listlen(l) == 1);
    l = cons("bar", l);
    fail_unless(listlen(l) == 2);
}
END_TEST

Suite *test_list_suite ()
{
    Suite *s = suite_create ("list");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_list_length);
    suite_add_tcase (s, tc_core);

    return s;
}


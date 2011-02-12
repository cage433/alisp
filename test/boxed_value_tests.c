#include "boxed_value.h"
#include "check.h"
#include "test_suites.h"

START_TEST(test_boxed_value_equality)
{
    fail_unless(boxed_values_equal(make_boxed_int(3), make_boxed_int(3)));
    fail_if(boxed_values_equal(make_boxed_int(4), make_boxed_int(3)));

    fail_unless(boxed_values_equal(make_boxed_double(3.5), make_boxed_double(3.5)));
    fail_if(boxed_values_equal(make_boxed_double(4.2), make_boxed_double(3.5)));
    fail_if(boxed_values_equal(make_boxed_int(4), make_boxed_double(4.0)));

    fail_unless(boxed_values_equal(make_boxed_string("fred"), make_boxed_string("fred")));
    fail_if(boxed_values_equal(make_boxed_string("mike"), make_boxed_string("fred")));

    fail_unless(boxed_values_equal(
        make_boxed_cons(make_boxed_string("a"), make_boxed_string("b")),
        make_boxed_cons(make_boxed_string("a"), make_boxed_string("b"))));

    fail_if(boxed_values_equal(
        make_boxed_cons(make_boxed_string("a"), make_boxed_string("b")),
        make_boxed_cons(make_boxed_string("a"), make_boxed_string("c"))));
}
END_TEST

START_TEST(test_null_list)
{
    fail_unless(boxed_values_equal(
        NIL,
        NIL
        ));
}
END_TEST

Suite *test_boxed_value_suite()
{
    Suite *s = suite_create ("boxed value");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_boxed_value_equality);
    tcase_add_test (tc_core, test_null_list);
    suite_add_tcase (s, tc_core);

    return s;
}

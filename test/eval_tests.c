#include "eval.h"
#include "check.h"
#include "environment.h"
#include "boxed_value.h"

START_TEST(test_eval_int)
{
    Env *env = create_env();
    expression *exp = make_integer_expression(12);
    boxed_value *v = eval(env, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(12)));
} 
END_TEST

Suite *test_eval_suite ()
{
    Suite *s = suite_create ("Eval tests");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_eval_int);
    suite_add_tcase (s, tc_core);

    return s;
}


#include "eval.h"
#include "check.h"
#include "environment.h"
#include "boxed_value.h"
#include "expression.h"
#include "parser.h"

START_TEST(test_eval_int)
{
    Env *env = create_env();
    expression *exp = make_integer_expression(12);
    boxed_value *v = eval(env, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(12)));
} 
END_TEST

START_TEST(test_eval_plus)
{
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(+ )")), 
        make_boxed_int(0)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(+ 4 5)")), 
        make_boxed_int(9)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(+ 4.0 5)")), 
        make_boxed_double(9.0)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(+ 10 4.0 5)")),
        make_boxed_double(19.0)));
} 
END_TEST

START_TEST(test_eval_times)
{
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(* )")), 
        make_boxed_int(1)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(* 4 5)")), 
        make_boxed_int(20)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(* 4.0 5)")), 
        make_boxed_double(20.0)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(* 10 4.0 5)")),
        make_boxed_double(200.0)));
} 
END_TEST

START_TEST(test_eval_minus)
{
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(- )")), 
        make_boxed_int(0)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(- 4 5)")), 
        make_boxed_int(-1)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(- 4.0 5)")), 
        make_boxed_double(-1.0)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(- 10 4.0 5)")),
        make_boxed_double(1.0)));
} 
END_TEST

START_TEST(test_eval_divide)
{
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(/ )")), 
        make_boxed_int(1)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(/ 4 5)")), 
        make_boxed_double(0.8)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(/ 4 2)")), 
        make_boxed_int(2)));
    fail_unless(boxed_values_equal(
        eval(create_env(), parse_expression_from_string("(/ 10 4.0 5)")),
        make_boxed_double(0.5)));
} 
END_TEST

START_TEST(test_eval_def)
{
    Env *env = create_env();
    expression *exp = parse_expression_from_string("(def foo (x) (+ x 1))");
    eval(env, exp);
    exp = parse_expression_from_string("(foo 10)");
    boxed_value *v = eval(env, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(11)));
}
END_TEST

START_TEST(test_eval_def2)
{
    Env *env = create_env();
    expression *exp = parse_expression_from_string("(def foo (x) (+ x 1))");
    eval(env, exp);
    exp = parse_expression_from_string("(def bar (x y) (+ x (foo y)))");
    eval(env, exp);
    exp = parse_expression_from_string("(bar 10 20)");
    boxed_value *v = eval(env, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(31)));
}
END_TEST

START_TEST(test_if_and_equals)
{
    Env *env = create_env();
    expression *exp = parse_expression_from_string("(def foo (n) (if (eq n 1) 10 20))");
    eval(env, exp);
    exp = parse_expression_from_string("(foo 5)");
    boxed_value *v = eval(env, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(20)));

    exp = parse_expression_from_string("(foo 1)");
    v = eval(env, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(10)));
}
END_TEST


START_TEST(test_factorial)
{
    Env *env = create_env();
    expression *exp = parse_expression_from_string("(def factorial (n) (if (eq n 1) 1 (* (factorial (- n 1)) n)))");
    eval(env, exp);
    exp = parse_expression_from_string("(factorial 5)");
    boxed_value *v = eval(env, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(120)));
}
END_TEST

START_TEST(test_and)
{
    Env *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("(and 3 4)");
    fail_unless(boxed_values_equal(make_boxed_int(4), eval(env, exp)));

    exp = parse_expression_from_string("(and NIL TRUE)");
    fail_unless(boxed_values_equal(NIL, eval(env, exp)));

    exp = parse_expression_from_string("(and TRUE NIL TRUE)");
    fail_unless(boxed_values_equal(NIL, eval(env, exp)));

    exp = parse_expression_from_string("(and)");
    fail_unless(boxed_values_equal(TRUE, eval(env, exp)));

    // UNASSIGNED_VARIABLE never evaluated
    exp = parse_expression_from_string("(and TRUE NIL UNASSIGNED_VARIABLE)");
    fail_unless(boxed_values_equal(NIL, eval(env, exp)));
}
END_TEST

START_TEST(test_or)
{
    Env *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("(or 3 4)");
    fail_unless(boxed_values_equal(make_boxed_int(3), eval(env, exp)));

    exp = parse_expression_from_string("(or NIL TRUE)");
    fail_unless(boxed_values_equal(TRUE, eval(env, exp)));

    exp = parse_expression_from_string("(or)");
    fail_unless(boxed_values_equal(NIL, eval(env, exp)));

    exp = parse_expression_from_string("(or NIL NIL)");
    fail_unless(boxed_values_equal(NIL, eval(env, exp)));

    // UNASSIGNED_VARIABLE never evaluated
    exp = parse_expression_from_string("(or TRUE NIL UNASSIGNED_VARIABLE)");
    fail_unless(boxed_values_equal(TRUE, eval(env, exp)));
}
END_TEST

START_TEST(test_cons)
{
    Env *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("(car (cons 3 4))");
    fail_unless(boxed_values_equal(make_boxed_int(3), eval(env, exp)));
    exp = parse_expression_from_string("(cdr (cons 3 4))");
    fail_unless(boxed_values_equal(make_boxed_int(4), eval(env, exp)));
}
END_TEST

START_TEST(test_lambda)
{
    Env *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("((lambda (x) (+ x 5)) 6)");
    fail_unless(boxed_values_equal(make_boxed_int(11), eval(env, exp)));
}
END_TEST

START_TEST(test_lambdas_can_be_returned_by_functions)
{
    Env *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("(def inc() (lambda (x) (+ x 10)))");
    eval(env, exp);
    exp = parse_expression_from_string("((inc) 4)");
    fail_unless(boxed_values_equal(make_boxed_int(14), eval(env, exp)));
}
END_TEST

Suite *test_eval_suite ()
{
    Suite *s = suite_create ("eval");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_eval_int);
    tcase_add_test (tc_core, test_eval_plus);
    tcase_add_test (tc_core, test_eval_times);
    tcase_add_test (tc_core, test_eval_minus);
    tcase_add_test (tc_core, test_eval_divide);
    tcase_add_test (tc_core, test_eval_def);
    tcase_add_test (tc_core, test_eval_def2);
    tcase_add_test (tc_core, test_eval_int);
    tcase_add_test (tc_core, test_factorial);
    tcase_add_test (tc_core, test_if_and_equals);
    tcase_add_test (tc_core, test_and);
    tcase_add_test (tc_core, test_or);
    tcase_add_test (tc_core, test_cons);
    tcase_add_test (tc_core, test_lambda);
    tcase_add_test (tc_core, test_lambdas_can_be_returned_by_functions);
    suite_add_tcase (s, tc_core);

    return s;
}


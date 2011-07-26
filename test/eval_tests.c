#include "eval.h"
#include "check.h"
#include "environment.h"
#include "boxed_value.h"
#include "expression.h"
#include "parser.h"

START_TEST(test_eval_int)
{
    List *env = create_env();
    expression *exp = make_integer_expression(12);
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(12)));
} 
END_TEST

START_TEST(test_eval_plus)
{
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(+ )")), 
            make_boxed_int(0)));
    fail_unless(boxed_values_equal(
                eval_exp(create_env(), NULL, parse_expression_from_string("(+ 4 5)")), 
                make_boxed_int(9)));
    fail_unless(boxed_values_equal(
                eval_exp(create_env(), NULL, parse_expression_from_string("(+ 4.0 5)")), 
                make_boxed_double(9.0)));
    fail_unless(boxed_values_equal(
                eval_exp(create_env(), NULL, parse_expression_from_string("(+ 10 4.0 5)")),
                make_boxed_double(19.0)));
} 
END_TEST

START_TEST(test_eval_times)
{
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(* )")), 
        make_boxed_int(1)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(* 4 5)")), 
        make_boxed_int(20)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(* 4.0 5)")), 
        make_boxed_double(20.0)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(* 10 4.0 5)")),
        make_boxed_double(200.0)));
} 
END_TEST

START_TEST(test_eval_minus)
{
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(- )")), 
        make_boxed_int(0)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(- 4 5)")), 
        make_boxed_int(-1)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(- 4.0 5)")), 
        make_boxed_double(-1.0)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(- 10 4.0 5)")),
        make_boxed_double(1.0)));
} 
END_TEST

START_TEST(test_eval_divide)
{
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(/ )")), 
        make_boxed_int(1)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(/ 4 5)")), 
        make_boxed_double(0.8)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(/ 4 2)")), 
        make_boxed_int(2)));
    fail_unless(boxed_values_equal(
        eval_exp(create_env(), NULL, parse_expression_from_string("(/ 10 4.0 5)")),
        make_boxed_double(0.5)));
} 
END_TEST

START_TEST(test_eval_def)
{
    List *env = create_env();
    expression *exp = parse_expression_from_string("(def (foo x) (+ x 1))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("(foo 10)");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(11)));
}
END_TEST

START_TEST(test_eval_def2)
{
    List *env = create_env();
    expression *exp = parse_expression_from_string("(def (foo x) (+ x 1))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("(def (bar x y) (+ x (foo y)))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("(bar 10 20)");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(31)));
}
END_TEST

START_TEST(test_if_and_equals)
{
    List *env = create_env();
    expression *exp = parse_expression_from_string("(def (foo n) (if (eq n 1) 10 20))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("(foo 5)");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(20)));

    exp = parse_expression_from_string("(foo 1)");
    v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(10)));
}
END_TEST

START_TEST(test_if_with_no_alternative)
{
    List *env = create_env();
    expression *exp = parse_expression_from_string("(def (foo n) (if (eq n 1) 10))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("(foo 5)");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(v, NIL));

    exp = parse_expression_from_string("(foo 1)");
    v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(10)));
}
END_TEST

START_TEST(test_factorial)
{
    List *env = create_env();
    expression *exp = parse_expression_from_string("(def (factorial n) (if (eq n 1) 1 (* (factorial (- n 1)) n)))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("(factorial 5)");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(v, make_boxed_int(120)));
}
END_TEST

START_TEST(test_and)
{
    List *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("(and 3 4)");
    fail_unless(boxed_values_equal(make_boxed_int(4), eval_exp(env, NULL, exp)));

    exp = parse_expression_from_string("(and NIL TRUE)");
    fail_unless(boxed_values_equal(NIL, eval_exp(env, NULL, exp)));

    exp = parse_expression_from_string("(and TRUE NIL TRUE)");
    fail_unless(boxed_values_equal(NIL, eval_exp(env, NULL, exp)));

    exp = parse_expression_from_string("(and)");
    fail_unless(boxed_values_equal(TRUE, eval_exp(env, NULL, exp)));

    // UNASSIGNED_VARIABLE never evaluated
    exp = parse_expression_from_string("(and TRUE NIL UNASSIGNED_VARIABLE)");
    fail_unless(boxed_values_equal(NIL, eval_exp(env, NULL, exp)));
}
END_TEST

START_TEST(test_or)
{
    List *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("(or 3 4)");
    fail_unless(boxed_values_equal(make_boxed_int(3), eval_exp(env, NULL, exp)));

    exp = parse_expression_from_string("(or NIL TRUE)");
    fail_unless(boxed_values_equal(TRUE, eval_exp(env, NULL, exp)));

    exp = parse_expression_from_string("(or)");
    fail_unless(boxed_values_equal(NIL, eval_exp(env, NULL, exp)));

    exp = parse_expression_from_string("(or NIL NIL)");
    fail_unless(boxed_values_equal(NIL, eval_exp(env, NULL, exp)));

    // UNASSIGNED_VARIABLE never evaluated
    exp = parse_expression_from_string("(or TRUE NIL UNASSIGNED_VARIABLE)");
    fail_unless(boxed_values_equal(TRUE, eval_exp(env, NULL, exp)));
}
END_TEST

START_TEST(test_cons)
{
    List *env;
    expression *exp;
    env = create_env();
    exp = parse_expression_from_string("(car (cons 3 4))");
    fail_unless(boxed_values_equal(make_boxed_int(3), eval_exp(env, NULL, exp)));
    exp = parse_expression_from_string("(cdr (cons 3 4))");
    fail_unless(boxed_values_equal(make_boxed_int(4), eval_exp(env, NULL, exp)));
}
END_TEST

START_TEST(test_lambda)
{
    List *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("((lambda (x) (+ x 5)) 6)");
    fail_unless(boxed_values_equal(make_boxed_int(11), eval_exp(env, NULL, exp)));
}
END_TEST

START_TEST(test_lambdas_can_be_returned_by_functions)
{
    List *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("(def (inc) (lambda (x) (+ x 10)))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("((inc) 4)");
    fail_unless(boxed_values_equal(make_boxed_int(14), eval_exp(env, NULL, exp)));
}
END_TEST

START_TEST(test_closure)
{
    List *env = create_env();
    expression *exp;
    exp = parse_expression_from_string("(def (inc n) (lambda (x) (+ x n)))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("((inc 5) 4)");
    fail_unless(boxed_values_equal(make_boxed_int(9), eval_exp(env, NULL, exp)));
}
END_TEST

START_TEST(test_variable_shadowed)
{
    List *env = create_env();
    expression *exp = parse_expression_from_string("(def (foo n) (lambda (n) n))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("((foo 5) 4)");
    fail_unless(boxed_values_equal(make_boxed_int(4), eval_exp(env, NULL, exp)));
}
END_TEST

START_TEST(test_set)
{
    List *env = create_env();
    expression *exp = parse_expression_from_string("(def x 20)");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("(set! x 10)");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("x");
    fail_unless(boxed_values_equal(make_boxed_int(10), eval_exp(env, NULL, exp)));
}
END_TEST

START_TEST(test_progn){
    List *env = create_env();
    expression *exp = parse_expression_from_string("(progn 3 4)");
    boxed_value *val = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(make_boxed_int(4), val));
}
END_TEST

START_TEST(test_def_inside_lambda){
    List *env = create_env();
    expression *exp = parse_expression_from_string("(def (foo) (progn (def x 10) x))");
    eval_exp(env, NULL, exp);
    exp = parse_expression_from_string("(foo)");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(make_boxed_int(10), v));
    v = eval_exp(env, NULL, parse_expression_from_string("x"));
    fail_unless(boxed_values_equal(make_boxed_int(10), v));
}
END_TEST

START_TEST(test_tagbody){
    List *env = create_env();
    expression *exp = parse_expression_from_string("(tagbody)");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(NIL, v));
}
END_TEST

START_TEST(test_tagbody2){
    List *env = create_env();
    expression *exp = parse_expression_from_string("(tagbody foo 3)");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(NIL, v));
}
END_TEST

START_TEST(test_tagbody3){
    List *env = create_env();
    eval_exp(env, NULL, parse_expression_from_string("(def x 3)"));
    eval_exp(env, NULL, parse_expression_from_string("(tagbody foo (if (eq x 10) 77 (progn (set! x (+ x 1)) (go foo))))")); 
    expression *exp = parse_expression_from_string("x");
    boxed_value *v = eval_exp(env, NULL, exp);
    fail_unless(boxed_values_equal(make_boxed_int(10), v));
}
END_TEST

START_TEST(test_comparators){
    List *env = create_env();
    int num_trues = 4;
    char *true_expressions[4] = {
        "(< 3 4)",
        "(<= 3 4)",
        "(<= 3 3)",
        "(< 3 4 5)"
    };
    int i;
    for (i = 0; i < num_trues; ++i){
        fail_unless(boxed_values_equal(
            TRUE,
            eval_exp(env, NULL, parse_expression_from_string(true_expressions[i]))
        ));
    }
    int num_falses = 5;
    char *false_expressions[5] = {
        "(< 3 3)",
        "(< 3.0 3.0)",
        "(> 3 3)",
        "(> 3 3.0)",
        "(< 3 4 5 3)"
    };

    for (i = 0; i < num_falses; ++i){
        fail_unless(boxed_values_equal(
            NIL,
            eval_exp(env, NULL, parse_expression_from_string(false_expressions[i]))
        ));
    }
}
END_TEST

START_TEST(test_tagbody_factorial){
    List *env = create_env();
    char *exp = "(def (fact n)                               \
                    ((lambda (i acc)                        \
                        (tagbody                            \
                            label                           \
                            (if (<= i n)                     \
                                (progn                      \
                                    (set! acc (* i acc))    \
                                    (set! i (+ i 1))        \
                                    (go label))))           \
                        acc)                            \
                    1 1) )";

    eval_exp(env, NULL, parse_expression_from_string(exp));
    boxed_value *v = eval_exp(env, NULL, parse_expression_from_string("(fact 4)")); 
    fail_unless(boxed_values_equal(make_boxed_int(24), v));
}
END_TEST

START_TEST(test_quote){
    List *env = create_env();
    char *exp = "(quote 4)";
    boxed_value *v = eval_exp(env, NULL, parse_expression_from_string(exp));
    boxed_value *v2 = eval_boxed(env, NULL, v);
    fail_unless(boxed_values_equal(make_boxed_int(4), v2));
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
    tcase_add_test (tc_core, test_if_with_no_alternative);
    tcase_add_test (tc_core, test_and);
    tcase_add_test (tc_core, test_or);
    tcase_add_test (tc_core, test_cons);
    tcase_add_test (tc_core, test_lambda);
    tcase_add_test (tc_core, test_lambdas_can_be_returned_by_functions);
    tcase_add_test (tc_core, test_closure);
    tcase_add_test (tc_core, test_variable_shadowed);
    tcase_add_test (tc_core, test_set);
    tcase_add_test (tc_core, test_progn);
    tcase_add_test (tc_core, test_def_inside_lambda);
    tcase_add_test (tc_core, test_tagbody);
    tcase_add_test (tc_core, test_tagbody2);
    tcase_add_test (tc_core, test_tagbody3);
    tcase_add_test (tc_core, test_comparators);
    tcase_add_test (tc_core, test_tagbody_factorial);
    tcase_add_test (tc_core, test_quote);
    suite_add_tcase (s, tc_core);

    return s;
}


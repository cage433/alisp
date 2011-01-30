#include "check.h"
#include "parser.h"
#include "list.h"
#include "stdio.h"
#include "expression.h"

static char* code[] = {
        "10", 
        "10 12",
        "10.5",
        "10.5 19"
};


START_TEST(test_parse_int)
{
        expression *expected = make_integer_expression(10);        
        char *code = "10";
        FILE *s = fmemopen(code, strlen(code), "r");
        List *l = parse_expressions(s);
        fail_unless(listlen(l) == 1);
        fail_unless(expressions_equal(l->car, expected));
}
END_TEST

START_TEST(test_parse_two_ints)
{
        expression *expected = make_integer_expression(10);        
        char *code = "10 12";
        FILE *s = fmemopen(code, strlen(code), "r");
        List *l = parse_expressions(s);
        fail_unless(listlen(l) == 2);
        fail_unless(expressions_equal(l->car, make_integer_expression(10)));
        fail_unless(expressions_equal(l->cdr->car, make_integer_expression(12)));
}
END_TEST

START_TEST(test_parse_double)
{
        char *code = "10.5";
        FILE *s = fmemopen(code, strlen(code), "r");
        List *l = parse_expressions(s);
        fail_unless(listlen(l) == 1);
        fail_unless(expressions_equal(l->car, make_double_expression(10.5)));
}
END_TEST

START_TEST(test_parse_double_and_int)
{
        char *code = "10.5 19";
        FILE *s = fmemopen(code, strlen(code), "r");
        List *l = parse_expressions(s);
        fail_unless(listlen(l) == 2);
        fail_unless(expressions_equal(l->car, make_double_expression(10.5)));
        fail_unless(expressions_equal(l->cdr->car, make_integer_expression(19)));
}
END_TEST

Suite *test_parser_suite()
{
        Suite *s = suite_create ("parser");

        /* Core test case */
        TCase *tc_core = tcase_create ("Core");
        tcase_add_test (tc_core, test_parse_int);
        tcase_add_test (tc_core, test_parse_two_ints);
        tcase_add_test (tc_core, test_parse_double);
        tcase_add_test (tc_core, test_parse_double_and_int);
        suite_add_tcase (s, tc_core);

        return s;
}


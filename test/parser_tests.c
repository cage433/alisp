#include "check.h"
#include "parser.h"
#include "list.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "expression.h"

static int num_tests = 12;
static char* codes[] = {
    "10", 
    "10 12",
    "10.5",
    "10.5 19",
    "fred",
    "mike 13 12.2",
    "(foo)",
    "(foo 1)",
    "(foo 1 9.5)",
    "(foo 1 (bar mike))",
    "(def (foo x) x)",
    "(def (foo x y) (+ x y))"
};
static List **expected = NULL;

void initialise_expected(){
    if (expected != NULL)
        return;
    expected = my_malloc(num_tests * sizeof(List *));
    expected[0] = make_list(1, make_integer_expression(10));
    expected[0] = make_list(1, make_integer_expression(10));
    expected[1] = make_list(2, make_integer_expression(10), make_integer_expression(12));
    expected[2] = make_list(1, make_double_expression(10.5));
    expected[3] = make_list(2, make_double_expression(10.5), make_integer_expression(19));
    expected[4] = make_list(1, make_identifier_expression("fred"));
    expected[5] = make_list(3, make_identifier_expression("mike"), make_integer_expression(13), make_double_expression(12.2));
    expected[6] = make_list(1, make_call_expression(make_identifier_expression("foo"), make_list(0)));
    expected[7] = make_list(1, make_call_expression(make_identifier_expression("foo"), make_list(1, make_integer_expression(1))));
    expected[8] = make_list(1, make_call_expression(make_identifier_expression("foo"), make_list(2, make_integer_expression(1), make_double_expression(9.5))));
    expected[9] = make_list(1, 
                make_call_expression(make_identifier_expression("foo"), 
                    make_list(2, 
                        make_integer_expression(1), 
                        make_call_expression(make_identifier_expression("bar"), 
                            make_list(1, 
                                make_identifier_expression("mike"))))));
    expected[10] = make_list(1, make_definition_expression("foo", make_function_expression(make_list(1, "x"), 
                    make_progn_expression(make_list(1, make_identifier_expression("x"))))));
    expected[11] = make_list(1, make_definition_expression(
                                    "foo", 
                                    make_function_expression(
                                        make_list(2, "x", "y"), 
                                        make_progn_expression(
                                            make_list(
                                                1, 
                                                make_call_expression(make_identifier_expression("+"), make_list(2, make_identifier_expression("x"), make_identifier_expression("y")))))
                                    )
                                    ));
}

List *expected_list(int i){
    initialise_expected();
    die_unless(i < num_tests, "Expected not written");
    return expected[i];
}


START_TEST(test_parser)
{
    List *expected = expected_list(_i);
    char *code = codes[_i];
    FILE *s = fmemopen(code, strlen(code), "r");
    List *l = parse_expressions(s, 1);
    fail_unless(listlen(l) == listlen(expected));
    fail_unless(lists_equal(expected, l, expressions_equal));
}
END_TEST

Suite *test_parser_suite()
{
    Suite *s = suite_create ("parser");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_loop_test (tc_core, test_parser, 0, num_tests);
    suite_add_tcase (s, tc_core);

    return s;
}


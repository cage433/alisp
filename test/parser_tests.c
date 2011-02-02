#include "check.h"
#include "parser.h"
#include "list.h"
#include "stdio.h"
#include "stdlib.h"
#include "expression.h"

static int num_tests = 10;
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
    "(foo 1 (bar mike))"
};

List *expected_list(int i){
    if (i == 0)
        return make_list(1, make_integer_expression(10));
    else if (i == 1)
        return make_list(2, make_integer_expression(10), make_integer_expression(12));
    else if (i == 2)
        return make_list(1, make_double_expression(10.5));
    else if (i == 3)
        return make_list(2, make_double_expression(10.5), make_integer_expression(19));
    else if (i == 4)
        return make_list(1, make_identifier_expression("fred"));
    else if (i == 5)
        return make_list(3, make_identifier_expression("mike"), make_integer_expression(13), make_double_expression(12.2));
    else if (i == 6)
        return make_list(1, make_call_expression("foo", make_list(0)));
    else if (i == 7)
        return make_list(1, make_call_expression("foo", make_list(1, make_integer_expression(1))));
    else if (i == 8)
        return make_list(1, make_call_expression("foo", make_list(2, make_integer_expression(1), make_double_expression(9.5))));
    else if (i == 9)
        return make_list(1, 
                make_call_expression("foo", 
                    make_list(2, 
                        make_integer_expression(1), 
                        make_call_expression("bar", 
                            make_list(1, 
                                make_identifier_expression("mike"))))));
    else {
        printf("File %s, line %d\n", __FILE__, __LINE__); 
        printf("Unexpected list %d - exiting\n", i);
        exit(-1);
    }
}


START_TEST(test_parser)
{
    List *expected = expected_list(_i);
    char *code = codes[_i];
    FILE *s = fmemopen(code, strlen(code), "r");
    List *l = parse_expressions(s);
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


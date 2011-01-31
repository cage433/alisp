#include "check.h"
#include "parser.h"
#include "list.h"
#include "stdio.h"
#include "stdlib.h"
#include "expression.h"

static char* codes[] = {
        "10", 
        "10 12",
        "10.5",
        "10.5 19"
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
        else {
                printf("File %s, line %d\n", __FILE__, __LINE__); 
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
        tcase_add_loop_test (tc_core, test_parser, 0, 4);
        suite_add_tcase (s, tc_core);

        return s;
}


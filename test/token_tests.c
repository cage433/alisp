#include "check.h"
#include "token.h"
#include "expression.h"
#include "lexer.h"
#include "list.h"
#include "test_suites.h"


START_TEST(test_make_token_list)
{
        List *list = make_list(2, &LEFT_PAREN, &RIGHT_PAREN);
        fail_unless(tokens_equal(&LEFT_PAREN, list->car));
}
END_TEST

static char *codes[] = {
        "(", 
        "(45",
        "( 45",
        "34.6)",
        "(fred)",
        "(x(",
        "( \
                x\
                (\
                ",
};
List *expected_lists(int i){
        switch (i){
                case 0:
                        return make_list(1, &LEFT_PAREN);
                case 1:
                case 2:
                        return make_list(2, &LEFT_PAREN, integer_token(45));
                case 3:
                        return make_list(2, double_token(34.6), &RIGHT_PAREN);
                case 4:
                        return make_list(3, &LEFT_PAREN, identifier_token("fred"), &RIGHT_PAREN);
                case 5:
                case 6:
                        return make_list(3, &LEFT_PAREN, identifier_token("x"), &LEFT_PAREN);
        }
}

START_TEST(test_lexer)
{
        char *code = codes[_i];
        FILE *stream;
        stream = fmemopen(code, strlen(code), "r");
        List *tokens = getTokens(stream);
        List *expected_list = expected_lists(_i);
        fail_unless(token_lists_equal(expected_list, tokens));
}
END_TEST


Suite *test_token_suite (void)
{
        Suite *s = suite_create ("alisp");

        /* Core test case */
        TCase *tc_core = tcase_create ("Core");
        tcase_add_test (tc_core, test_make_token_list);
        tcase_add_loop_test (tc_core, test_lexer, 0, 7);
        suite_add_tcase (s, tc_core);

        return s;
}


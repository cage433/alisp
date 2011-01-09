#include "stdlib.h"
#include "check.h"
#include "stdio.h"

#include "token.h"
#include "lexer.h"

char *TMPFILE = (char *)"tmpfile";
void write_str_to_tmp_file(char *string){
        FILE *file = fopen(TMPFILE, "w");
        fputs(string, file);
        fclose(file);
}

START_TEST(test_make_token_list)
{
        token_list *list = make_token_list(2, LEFT_PAREN, RIGHT_PAREN);
        fail_unless(tokens_equal(LEFT_PAREN, list->car));
}
END_TEST

static char *codes[] = {
        (char *)"(", 
        (char *)"(45",
        (char *)"( 45",
        (char *)"34.6)",
        (char *)"(fred)",
        (char *)"(x(",
        (char *)"( \
                x\
                (\
                ",
};
token_list *expected_lists(int i){
        switch (i){
                case 0:
                        return make_token_list(1, LEFT_PAREN);
                case 1:
                case 2:
                        return make_token_list(2, LEFT_PAREN, integer_token(45));
                case 3:
                        return make_token_list(2, double_token(34.6), RIGHT_PAREN);
                case 4:
                        return make_token_list(3, LEFT_PAREN, identifier_token((char *)"fred"), RIGHT_PAREN);
                case 5:
                case 6:
                        return make_token_list(3, LEFT_PAREN, identifier_token((char *)"x"), LEFT_PAREN);
        }
}

START_TEST(test_lexer)
{
        char *code = codes[_i];
        write_str_to_tmp_file(code);
        FILE *file = fopen(TMPFILE, "r");
        token_list *tokens = getTokens(file);
        token_list *expected_list = expected_lists(_i);
        fail_unless(token_lists_equal(expected_list, tokens));
}
END_TEST


Suite *
test_suite (void)
{
        Suite *s = suite_create ("alisp");

        /* Core test case */
        TCase *tc_core = tcase_create ("Core");
        tcase_add_test (tc_core, test_make_token_list);
        tcase_add_loop_test (tc_core, test_lexer, 0, 7);
        suite_add_tcase (s, tc_core);

        return s;
}


int main (void)
{
        int number_failed;
        Suite *s = test_suite ();
        SRunner *sr = srunner_create (s);
        srunner_run_all (sr, CK_NORMAL);
        number_failed = srunner_ntests_failed (sr);
        srunner_free (sr);
        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

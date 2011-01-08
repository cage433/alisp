#include "stdlib.h"
#include "check.h"
#include "alisp.h"
#include "stdio.h"

char *TMPFILE = "tmpfile";
void write_str_to_tmp_file(char *string){
        FILE *file = fopen(TMPFILE, "w");
        fputs(string, file);
        fclose(file);
}

START_TEST(single_paren)
{
        char *code = "(";
        write_str_to_tmp_file(code);
        FILE *file = fopen(TMPFILE, "r");
        token_list *tokens = getTokens(file);
        token_list *expected_list = make_token_list(1, LEFT_PAREN);
        fail_unless(token_lists_equal(expected_list, tokens));
}
END_TEST

START_TEST(two_tokens)
{
        char *code = "( 45";
        write_str_to_tmp_file(code);
        FILE *file = fopen(TMPFILE, "r");
        token_list *tokens = getTokens(file);
        typed_token tok45;
        tok45.type = tok_integer;
        tok45.intValue =  45;
        token_list *expected_list = make_token_list(2, LEFT_PAREN, tok45);
        fail_unless(token_lists_equal(expected_list, tokens));
}
END_TEST

Suite *
test_suite (void)
{
        Suite *s = suite_create ("alisp");

        /* Core test case */
        TCase *tc_core = tcase_create ("Core");
        tcase_add_test (tc_core, single_paren);
        tcase_add_test (tc_core, two_tokens);
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

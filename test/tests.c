#include "stdlib.h"
#include "check.h"
#include "stdio.h"

#include "test_suites.h"


int main ()
{
    int number_failed;
    SRunner *sr = srunner_create (test_token_suite());
    srunner_add_suite (sr, test_list_suite ());
    srunner_add_suite (sr, test_parser_suite());
    srunner_add_suite (sr, test_boxed_value_suite());
    srunner_add_suite (sr, test_hash_suite());
    srunner_add_suite (sr, test_eval_suite());
    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

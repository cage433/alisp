#include "stdlib.h"
#include "check.h"
#include "stdio.h"

#include "test_suites.h"


int main (void)
{
        int number_failed;
        Suite *s = test_token_suite ();
        SRunner *sr = srunner_create (s);
        srunner_add_suite (sr, test_list_suite ());
        srunner_run_all (sr, CK_NORMAL);
        number_failed = srunner_ntests_failed (sr);
        srunner_free (sr);
        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

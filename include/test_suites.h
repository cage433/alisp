#ifndef ALISP_TEST_SUITES
#define ALISP_TEST_SUITES

#include "check.h"


Suite *test_token_suite ();
Suite *test_list_suite ();
Suite *test_parser_suite();
Suite *test_boxed_value_suite();
Suite *test_hash_suite();
Suite *test_eval_suite();
Suite *test_frame_suite();

#endif

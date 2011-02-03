#include "check.h"
#include "hash.h"

void foo(){
    Hash *hash = hash_create(string_hash_fn, hash_strcmp);
}
START_TEST(test_hash_operations)
{
    Hash *hash = hash_create(string_hash_fn, hash_strcmp);
}
END_TEST

Suite *test_hash_suite()
{
    Suite *s = suite_create ("hash");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_hash_operations);
    suite_add_tcase (s, tc_core);

    return s;
}


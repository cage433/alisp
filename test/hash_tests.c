#include "check.h"
#include "hash.h"

START_TEST(test_hash_add_remove)
{
    Hash *hash = hash_create(string_hash_fn, hash_string_equality);
    fail_unless(hash->num_elements == 0);
    fail_if(hash_contains(hash, "fred"));
    hash_add(hash, "fred", "mike");
    fail_unless(hash_contains(hash, "fred"));
    fail_unless(hash->num_elements == 1);
    hash_remove(hash, "fred");
    fail_if(hash_contains(hash, "fred"));
    fail_unless(hash->num_elements == 0);
}
END_TEST

Suite *test_hash_suite()
{
    Suite *s = suite_create ("hash");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_hash_add_remove);
    suite_add_tcase (s, tc_core);

    return s;
}


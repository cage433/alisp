#include "check.h"
#include "hash.h"
#include "stdlib.h"
#include "stdio.h"

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

START_TEST(test_hash_add_lots_of_keys){
    Hash *hash = hash_create(string_hash_fn, hash_string_equality);
    int n = 100;
    char *keys[n];
    int i;
    for (i = 0; i < n; ++i){
        keys[i] = malloc(sizeof(char) * 10);
        sprintf(keys[i], "%d", i);
        fail_if(hash_contains(hash, keys[i]));
        hash_add(hash, keys[i], keys[i]);
        fail_unless(hash_contains(hash, keys[i]));
        fail_unless(hash->num_elements == i + 1);
    }
    for (i = 0; i < n; ++i){
        hash_remove(hash, keys[i]);
        fail_if(hash_contains(hash, keys[i]));
        fail_unless(hash->num_elements == n - 1 - i);
    }
}
END_TEST

Suite *test_hash_suite()
{
    Suite *s = suite_create ("hash");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test (tc_core, test_hash_add_remove);
    tcase_add_test (tc_core, test_hash_add_lots_of_keys);
    suite_add_tcase (s, tc_core);

    return s;
}


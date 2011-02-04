#ifndef ALISP_HASH
#define ALISP_HASH

#include "string.h"
#include "list.h"

typedef struct {
    void *key;
    void *value;
} KeyValuePair;

typedef struct {
    int array_length;
    int num_elements;
    List **array;
    long(*hashfn)(const void *key);
    int(*keyeq_fn)(const void *key1, const void *key2);
} Hash;


void hash_add(Hash *hash, void *key, void *value);
KeyValuePair *hash_remove(Hash *hash, void *key);
int hash_contains(Hash *hash, void *key);
void *hash_value(Hash *hash, void *key);
int hash_string_equality(const void *v1, const void * v2);
long string_hash_fn(const void *key);

Hash *hash_create(long(*hashfn)(const void *key), int(*keyeq_fn)(const void *key1, const void *key2));
#endif

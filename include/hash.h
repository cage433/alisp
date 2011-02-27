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
long string_hash_fn(const void *key);

Hash *hash_create(
        long(*hashfn)(const void *key), 
        int(*keyeq_fn)(const void *key1, const void *key2)
);
List *hash_keys(Hash *hash);
List *hash_values(Hash *hash);
void free_hash(Hash *hash, void(*key_value_free_fn)(KeyValuePair *));
Hash *copy_hash(Hash *hash);
void print_hash(Hash *hash, int indent);
#endif

#ifndef ALISP_HASH
#define ALISP_HASH

#include "list.h"

typedef struct {
    void *key;
    void *value;
} KeyValuePair;

typedef struct {
    int array_length;
    int num_elements;
    List **array;
    int(*hashfn)(void *key);
    int(*keyeq_fn)(void *key1, void *key2);
} Hash;

Hash *create_hash(int(*hashfn)(void *key), int(*keyeq_fn)(void *key1, void *key2));
#endif

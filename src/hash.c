#include "hash.h"

#include "stdlib.h"

Hash *create_hash(int(*hashfn)(void *key), int(*keyeq_fn)(void *key1, void *key2)){
    Hash *hash = (Hash *)malloc(sizeof(Hash));
    hash->array_length = 16;
    hash->num_elements = 0;
    hash->array = (List **)calloc(hash->array_length, sizeof(List *));
    hash->hashfn = hashfn;
    hash->keyeq_fn = keyeq_fn;
    return hash;
}

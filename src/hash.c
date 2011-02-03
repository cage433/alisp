#include "hash.h"

#include "stdlib.h"

Hash *hash_create(long(*hashfn)(const void *key), int(*keyeq_fn)(const void *key1, const void *key2)){
    Hash *hash = (Hash *)malloc(sizeof(Hash));
    hash->array_length = 16;
    hash->num_elements = 0;
    hash->array = (List **)calloc(hash->array_length, sizeof(List *));
    hash->hashfn = hashfn;
    hash->keyeq_fn = keyeq_fn;
    return hash;
}


int hash_strcmp(const void *v1, const void * v2){
    return strcmp((char *)v1, (char *)v2);
}
long string_hash_fn(const void *key){
    return 0;
}

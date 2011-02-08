#include "hash.h"

#include "stdlib.h"
#include "stdio.h"
#include "utils.h"
#include "boxed_value.h"

Hash *hash_create(long(*hashfn)(const void *key), int(*keyeq_fn)(const void *key1, const void *key2)){
    Hash *hash = (Hash *)malloc(sizeof(Hash));
    hash->array_length = 16;
    hash->num_elements = 0;
    hash->array = (List **)calloc(hash->array_length, sizeof(List *));
    hash->hashfn = hashfn;
    hash->keyeq_fn = keyeq_fn;
    return hash;
}

long string_hash_fn(const void *key){
    const char *k = key;
    size_t len = strlen(key);
    long hash = 0;
    int i = 0;
    int d;
    int j = 1;
    while (i < len){
        for (d = 0; d < 10; ++d){
            if (i < len){
                hash += k[i];
                hash *= 37;
            }
            i += j;
        }
        j *= 10;
    }
    return hash;
}

long hash_index(Hash *hash, void *key){
    long i = abs(ldiv(hash->hashfn(key), hash->array_length).rem);
    die_unless(i >= 0 && i < hash->array_length, "Invalid quotient");
    return i;
}

KeyValuePair *hash_key_value_pair(Hash *hash, void *key){
    long i = hash_index(hash, key);
    List *l = hash->array[i];
    while (l != NULL){
        KeyValuePair *kv = l->car;
        if (hash->keyeq_fn(key, kv->key))
            return kv;
        l = l->cdr;
    }
    return NULL;
}

KeyValuePair* hash_remove(Hash *hash, void *key){
    long i = hash_index(hash, key);
    List *l = hash->array[i];
    if (l == NULL)
        return NULL;
    KeyValuePair *kv = l->car;
    if (hash->keyeq_fn(key, kv->key)){
        hash->array[i] = l->cdr;
        free(l);
        hash->num_elements -= 1;
        return kv;
    } else {
        List *l2 = l->cdr;
        while (l2 != NULL){
            kv = l2->car;
            if (hash->keyeq_fn(key, kv->key)){
                l->cdr = l2->cdr;
                free(l2);
                hash->num_elements -= 1;
                return kv;
            } else {
                l = l2;
                l2 = l2->cdr;
            }
        }
    }
    return NULL;
}

int hash_contains(Hash *hash, void *key){
    return hash_key_value_pair(hash, key) != NULL;
}

void hash_resize(Hash *hash){
    int old_array_length = hash->array_length;
    hash->array_length *= 2;
    List **old_array = hash->array;

    hash->array = calloc(hash->array_length, sizeof(List *));
    hash->num_elements = 0;
    void add_to_hash(KeyValuePair *kv){
        hash_add(hash, kv->key, kv->value);
    }
    int i;
    for (i = 0; i < old_array_length; ++i){
        list_for_each(old_array[i], (for_each_fn_ptr)add_to_hash);
        free_list(old_array[i], free);
    }
}


void hash_add(Hash *hash, void *key, void *value){
    if (hash_contains(hash, key))
        hash_remove(hash, key);
    if (hash->num_elements * 2 > hash->array_length)
        hash_resize(hash);
    long i = hash_index(hash, key);
    KeyValuePair *kv = malloc(sizeof(KeyValuePair));
    kv->key = key;
    kv->value = value;
    hash->array[i] = cons(kv, hash->array[i]);
    hash->num_elements += 1;
}

List *hash_keys(Hash *hash){
    int i;
    List *keys = NULL;
    void add_key(KeyValuePair *kv){
        keys = cons(kv->key, keys);
    }
    for (i = 0; i < hash->array_length; ++i){
        list_for_each(hash->array[i], (for_each_fn_ptr)add_key);
//        List *l = hash->array[i];
//        while (l != NULL){
//            KeyValuePair *kv = l->car;
//            keys = cons(kv->key, keys);
//            l = l->cdr;
//        }
    }
    return keys;
}

void *hash_value(Hash *hash, void *key){
    KeyValuePair *kv = hash_key_value_pair(hash, key);
    die_unless(kv != NULL, "Key not found");
    return kv->value;
}

void free_hash(Hash *hash, void(*key_value_free_fn)(void *)){
    int i;
    for (i = 0; i < hash->array_length; ++i)
        free_list(hash->array[i], key_value_free_fn);
    free(hash->array);
    free(hash);
}

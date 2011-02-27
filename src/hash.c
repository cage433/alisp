#include "hash.h"

#include "stdlib.h"
#include "stdio.h"
#include "utils.h"
#include "boxed_value.h"

Hash *hash_create(long(*hashfn)(const void *key), int(*keyeq_fn)(const void *key1, const void *key2)){
    Hash *hash = my_malloc(sizeof(Hash));
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
    int key_matches(KeyValuePair *kv){
        return hash->keyeq_fn(key, kv->key);
    }
    return list_find(hash->array[i], (predicate_fn_ptr)key_matches);
}

KeyValuePair* hash_remove(Hash *hash, void *key){
    long i = hash_index(hash, key);
    List *l = hash->array[i];
    if (l == NULL)
        return NULL;
    KeyValuePair *kv = l->car;
    if (hash->keyeq_fn(key, kv->key)){
        hash->array[i] = l->cdr;
        my_free(l);
        hash->num_elements -= 1;
        return kv;
    } else {
        List *l2 = l->cdr;
        while (l2 != NULL){
            kv = l2->car;
            if (hash->keyeq_fn(key, kv->key)){
                l->cdr = l2->cdr;
                my_free(l2);
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
        free_list(old_array[i], (free_fn_ptr)my_free);
    }
}


void hash_add(Hash *hash, void *key, void *value){
    if (hash_contains(hash, key))
        hash_remove(hash, key);
    if (hash->num_elements * 2 > hash->array_length)
        hash_resize(hash);
    long i = hash_index(hash, key);
    KeyValuePair *kv = my_malloc(sizeof(KeyValuePair));
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
    }
    return keys;
}

void *hash_value(Hash *hash, void *key){
    KeyValuePair *kv = hash_key_value_pair(hash, key);
    die_unless(kv != NULL, "Key not found");
    return kv->value;
}

void free_hash(Hash *hash, void(*key_value_free_fn)(KeyValuePair *)){
    int i;
    for (i = 0; i < hash->array_length; ++i)
        free_list(hash->array[i], (free_fn_ptr)key_value_free_fn);
    my_free(hash->array);
    my_free(hash);
}

Hash *copy_hash(Hash *hash){
    Hash *copy = hash_create(
        hash->hashfn,
        hash->keyeq_fn
    );
    void add_key_value(void *key){
        void *value = hash_value(hash, key);
        hash_add(copy, key, value);
    }
        
    List *keys = hash_keys(hash);
    list_for_each(keys, add_key_value);
    free_list(keys, nop_free_fn);
    return copy;
}

List *hash_values(Hash *hash){
    List *keys = hash_keys(hash);
    void *get_hash_value(void *key){
        return hash_value(hash, key);
    }
    List *values = list_map(keys, (map_fn_ptr)get_hash_value);
    free_list(keys, nop_free_fn);
    return values;
}

void print_hash(Hash *hash, int indent){
    List *keys = hash_keys(hash);
    void print_key_and_value(void *key){
        print_tabs(indent);
        printf("Key = %s, val = ", (char *)key);
        boxed_value *val = hash_value(hash, key);
        print_boxed_value(val, 0);
    }
    list_for_each(keys, (for_each_fn_ptr)print_key_and_value);
    free_list(keys, nop_free_fn);
}


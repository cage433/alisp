#include "environment.h"
#include "utils.h"
#include "list.h"
#include "hash.h"
#include "boxed_value.h"
#include "stdlib.h"
#include "stdio.h"

Env *create_env(){
    Env *env = my_malloc(sizeof(Env));
    env->base = hash_create(string_hash_fn, strings_equal);
    hash_add(env->base, "NIL", NIL);
    hash_add(env->base, "TRUE", TRUE);
    env->frames = cons(env->base, NULL);
    return env;
}
void print_env(Env *env){
    List *frames = env->frames;
    while (frames != NULL){
        printf("Frame no %d\n", listlen(frames));
        void print_key_and_value(char *key){
            printf("Key %s, value ", key);
            print_boxed_value((boxed_value *)hash_value(frames->car, key));
        }
        list_for_each(hash_keys(frames->car), (for_each_fn_ptr)print_key_and_value);
        frames = frames->cdr;
    }
}

void env_add_frame(Env *env, Hash *frame){
    env->frames = cons(frame, env->frames);
}
static void free_binding(KeyValuePair *kv){
    my_free(kv->key);
    dec_ref_count((boxed_value *)kv->value);
    my_free(kv);
}

void env_drop_frame(Env *env, int free_frame){
    if (free_frame)
        free_hash(env->frames->car, free_binding);
    env->frames = env->frames->cdr;
}

boxed_value *env_lookup(Env *env, char *name){
    int hash_contains_name(Hash *hash){
        return hash_contains(hash, name);
    }
    Hash *frame = list_find(env->frames, (predicate_fn_ptr)hash_contains_name);
    if (frame == NULL){
        printf("Looking for %s\n", name);
        die("Value not found in environment");
    } else {
        return hash_value(frame, name);
    }
}

Hash *frame_create(List *args, List *values){
    Hash *frame = hash_create(string_hash_fn, strings_equal);
    while (args != NULL && values != NULL){
        hash_add(frame, strdup(args->car), values->car);
        inc_ref_count((boxed_value *)values->car);
        args = args->cdr;
        values = values->cdr;
    }
    die_unless(args == NULL && values == NULL, "Arg names and values have different lengths");
    return frame;
}

void inc_hash_value_ref_count(Hash *hash){
    List *values = hash_values(hash);
    list_for_each(values, (for_each_fn_ptr)inc_ref_count);
    free_list(values, nop_free_fn);
}

Env *copy_env(Env *env){
    Env *copy = malloc(sizeof(Env));
    copy->base = copy_hash(env->base);
    copy->frames = list_map(env->frames, (map_fn_ptr)copy_hash);
    inc_hash_value_ref_count(copy->base);
    list_map(copy->frames, (map_fn_ptr)inc_hash_value_ref_count);
    return copy;
}


int has_binding(Env *env, char *name){
    int frame_has_binding(Hash *frame){
        return hash_contains(frame, name);
    }
    return list_some(env->frames, (predicate_fn_ptr)frame_has_binding) || hash_contains(env->base, name);
}

Hash *collapse_to_single_frame(Env *env){
    Hash *single_frame = frame_create(NULL, NULL);
    void add_values_from_frame(Hash *frame){
        List *keys = hash_keys(frame);
        List *keys2 = keys;
        while (keys2 != NULL){
            char *key = keys2->car;
            if (! hash_contains(single_frame, key))
                hash_add(single_frame, key, hash_value(frame, key));
            keys2 = keys2->cdr;
        }
        free_list(keys, nop_free_fn);
    }
    list_for_each(env->frames, (for_each_fn_ptr)add_values_from_frame);
    return single_frame;
}



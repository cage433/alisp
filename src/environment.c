#include "environment.h"
#include "utils.h"
#include "list.h"
#include "hash.h"
#include "frame.h"
#include "boxed_value.h"
#include "stdlib.h"
#include "stdio.h"

List *create_env(){
    Hash *base = create_empty_frame();
    frame_add(base, "NIL", NIL);
    frame_add(base, "TRUE", TRUE);
    return cons(base, NULL);
}

void print_env(List *env, int indent){
    while (env != NULL){
        print_tabs(indent);
        printf("Frame no %d\n", listlen(env));
        void print_key_and_value(char *key){
            print_tabs(indent + 1);
            printf("Key %s, value ", key);
            print_boxed_value((boxed_value *)hash_value(env->car, key), 0);
        }
        list_for_each(hash_keys(env->car), (for_each_fn_ptr)print_key_and_value);
        env = env->cdr;
    }
}

List *env_add_frame(List *env, Hash *frame){
    return cons(frame, env);
}

List *env_drop_frame(List *env){
    free_frame(env->car);
    List *cdr = env->cdr;
    free(env);
    die_if(cdr == NULL, "dropping base frame");
    return cdr;
}

boxed_value *env_lookup(List *env, char *name){
    int hash_contains_name(Hash *hash){
        return hash_contains(hash, name);
    }
    Hash *frame = list_find(env, (predicate_fn_ptr)hash_contains_name)->car;
    if (frame == NULL){
        die(make_msg("Value %s not found in environment", name));
    } else {
        return hash_value(frame, name);
    }
}

List *copy_env(List *env){
    return list_map(env, (map_fn_ptr)copy_frame);
}


int env_has_binding(List *env, char *name){
    int frame_has_binding(Hash *frame){
        return hash_contains(frame, name);
    }
    return list_some(env, (predicate_fn_ptr)frame_has_binding);
}

Hash *collapse_to_single_frame(List *env){
    Hash *single_frame = create_empty_frame();
    void add_values_from_frame(Hash *frame){
        List *keys = hash_keys(frame);
        List *keys2 = keys;
        while (keys2 != NULL){
            char *key = keys2->car;
            if (! hash_contains(single_frame, key))
                frame_add(single_frame, key, hash_value(frame, key));
            keys2 = keys2->cdr;
        }
        free_list(keys, nop_free_fn);
    }
    list_for_each(env, (for_each_fn_ptr)add_values_from_frame);
    return single_frame;
}

void free_env(List *env){
    while (env != NULL)
        env = env_drop_frame(env);
}

void define_value_in_env(List *env, char *key, boxed_value *value){
    Hash *base = list_last_element(env);
    if (hash_contains(base, key))
        die(make_msg("Trying to redefine value for %s", key));
    frame_add(base, key, value);
}

void set_value_in_env(List *env, char *key, boxed_value *value){
    while (env != NULL){
        Hash *frame = (Hash *)env->car;
        if (hash_contains(frame, key)){
            frame_add(frame, key, value);
            return;
        } else {
            env = env->cdr;
        }
    }
    die(make_msg("No existing value of %s to set", key));
}


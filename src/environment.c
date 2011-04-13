#include "environment.h"
#include "utils.h"
#include "list.h"
#include "hash.h"
#include "frame.h"
#include "boxed_value.h"
#include "stdlib.h"
#include "stdio.h"

Env *create_env(){
    Env *env = my_malloc(sizeof(Env));
    env->base = create_empty_frame();
    frame_add(env->base, "NIL", NIL);
    frame_add(env->base, "TRUE", TRUE);
    env->frames = cons(env->base, NULL);
    return env;
}
void print_env(Env *env, int indent){
    List *frames = env->frames;
    while (frames != NULL){
        print_tabs(indent);
        printf("Frame no %d\n", listlen(frames));
        void print_key_and_value(char *key){
            print_tabs(indent + 1);
            printf("Key %s, value ", key);
            print_boxed_value((boxed_value *)hash_value(frames->car, key), 0);
        }
        list_for_each(hash_keys(frames->car), (for_each_fn_ptr)print_key_and_value);
        frames = frames->cdr;
    }
}

void env_add_frame(Env *env, Hash *frame){
    env->frames = cons(frame, env->frames);
}

void env_drop_frame(Env *env, int do_free_frame){
    if (do_free_frame)
        free_frame(env->frames->car);
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

Env *copy_env(Env *env){
    Env *copy = malloc(sizeof(Env));
    copy->base = copy_frame(env->base);
    copy->frames = list_map(env->frames, (map_fn_ptr)copy_frame);
    return copy;
}


int env_has_binding(Env *env, char *name){
    int frame_has_binding(Hash *frame){
        return hash_contains(frame, name);
    }
    return list_some(env->frames, (predicate_fn_ptr)frame_has_binding) || hash_contains(env->base, name);
}

Hash *collapse_to_single_frame(Env *env){
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
    list_for_each(env->frames, (for_each_fn_ptr)add_values_from_frame);
    return single_frame;
}

void free_env(Env *env){
    while (env->frames != NULL)
        env_drop_frame(env, 1);
}

void set_value_in_env(Env *env, char *key, boxed_value *value, int can_create_new_value){
    List *frames = env->frames;
    while (frames != NULL){
        Hash *frame = (Hash *)frames->car;
        if (hash_contains(frame, key)){
            frame_add(frame, key, value);
            return;
        } else {
            frames = frames->cdr;
        }
    }
    if (can_create_new_value)
        frame_add(env->frames->car, key, value);
    else
        die("No existing value to set");
}


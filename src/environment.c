#include "environment.h"
#include "utils.h"
#include "list.h"
#include "hash.h"
#include "boxed_value.h"
#include "stdlib.h"
#include "stdio.h"

Env *create_env(){
    Env *env = malloc(sizeof(Env));
    env->base = hash_create(string_hash_fn, strings_equal);
    env->frames = cons(env->base, NULL);
    return env;
}
void env_add_frame(Env *env, Hash *frame){
    env->frames = cons(frame, env->frames);
}
void env_drop_frame(Env *env){
    Hash *frame = env->frames->car;
    List *keys = hash_keys(frame);
    List *keys2 = keys;
    while (keys2 != NULL){
        char *key = keys2->car;
        boxed_value *value = hash_value(frame, key);
        free_boxed_value(value);
        keys2 = keys2->cdr;
    }
    free(keys);
    free_hash(frame);
    env->frames = env->frames->cdr;
    free(frame);
}

boxed_value *env_lookup(Env *env, char *name){
    List *l = env->frames;
    while (l != NULL){
        if (hash_contains(l->car, name))
            return hash_value(l->car, name);
        l = l->cdr;
    }
    printf("Looking for %s\n", name);
    die("Value not found in environment");
}

Hash *frame_create(List *args, List *values){
    Hash *frame = hash_create(string_hash_fn, strings_equal);
    while (args != NULL && values != NULL){
        hash_add(frame, args->car, values->car);
        args = args->cdr;
        values = values->cdr;
    }
    die_unless(args == NULL && values == NULL, "Arg names and values have different lengths");
    return frame;
}

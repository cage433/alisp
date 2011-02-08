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
    }
}

void env_add_frame(Env *env, Hash *frame){
    env->frames = cons(frame, env->frames);
}
static void free_binding(KeyValuePair *kv){
    free(kv->key);
    free_boxed_value((boxed_value *)kv->value);
    free(kv);
}

void env_drop_frame(Env *env){
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
        args = args->cdr;
        values = values->cdr;
    }
    die_unless(args == NULL && values == NULL, "Arg names and values have different lengths");
    return frame;
}

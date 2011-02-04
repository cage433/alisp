#include "environment.h"

#include "list.h"
#include "hash.h"
#include "boxed_value.h"
#include "stdlib.h"
List *create_env(){
    return NULL;
}
List *add_frame(List *env, Hash *frame){
    List *new_env = malloc(sizeof(List));
    new_env->car = frame;
    new_env->cdr = env;
}
List *drop_frame(List *env){
    Hash *frame = env->car;
    List *keys = hash_keys(frame);
    List *keys2 = keys;
    while (keys2 != NULL){
        char *key = keys2->car;
        boxed_value *value = hash_value(frame, key);
        free_boxed_value(value);
        free(key);
        free(value);
        keys2 = keys2->cdr;
    }
    free_hash(frame);
    List *remaining_env = env->cdr;
    free(frame);
    return remaining_env;
}

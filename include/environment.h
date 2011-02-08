#ifndef ALISP_ENVIRONMENT
#define ALISP_ENVIRONMENT

#include "list.h"
#include "hash.h"
#include "boxed_value.h"

typedef struct {
    Hash *base;
    List *frames;
} Env;
Env *create_env();
void env_add_frame(Env *env, Hash *frame);
void env_drop_frame(Env *env);
boxed_value *env_lookup(Env *env, char *name);
Hash *frame_create(List *args, List *values);
void print_env(Env *env);
#endif

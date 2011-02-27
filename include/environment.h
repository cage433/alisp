#ifndef ALISP_ENVIRONMENT
#define ALISP_ENVIRONMENT

#include "list.h"
#include "hash.h"

struct boxed_value;

typedef struct Env{
    Hash *base;
    List *frames;
} Env;
Env *create_env();
void env_add_frame(Env *env, Hash *frame);
void env_drop_frame(Env *env, int free_frame);


Hash *frame_create(List *args, List *values);
void print_env(Env *env, int indent);
struct boxed_value *env_lookup(Env *env, char *name);
Env *copy_env(Env *env);
int has_binding(Env *env, char *name);
Hash *collapse_to_single_frame(Env *env);
#endif

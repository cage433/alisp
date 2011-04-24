#ifndef ALISP_ENVIRONMENT
#define ALISP_ENVIRONMENT

#include "list.h"
#include "hash.h"

struct boxed_value;

List *create_env();
List *env_add_frame(List *env, Hash *frame);
List *env_drop_frame(List *env);


void print_env(List *env, int indent);
struct boxed_value *env_lookup(List *env, char *name);
List *copy_env(List *env);
int env_has_binding(List *env, char *name);
Hash *collapse_to_single_frame(List *env);
void free_env(List *env);
void set_value_in_env(List *env, char *key, struct boxed_value *value);
#endif

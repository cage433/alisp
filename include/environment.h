#ifndef ALISP_ENVIRONMENT
#define ALISP_ENVIRONMENT

#include "list.h"
#include "hash.h"

List *create_env();
List *add_frame(List *env, Hash *frame);
List *drop_frame(List *env);

#endif

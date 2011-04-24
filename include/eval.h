#ifndef ALISP_EVAL
#define ALISP_EVAL

#include "expression.h"
#include "boxed_value.h"
#include "environment.h"
#include "hash.h"

boxed_value *eval(List *env, List *tagbody_env_pairs, expression *exp);
boxed_value *apply(List *env, List *tagbody_env_pairs, expression *func, List *args);

#endif

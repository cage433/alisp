#ifndef ALISP_EVAL
#define ALISP_EVAL

#include "expression.h"
#include "boxed_value.h"
#include "environment.h"
#include "hash.h"
#include "list.h"

boxed_value *eval_boxed(List *env, List *tagbody_env_pairs, boxed_value *exp);
boxed_value *eval_exp(List *env, List *tagbody_env_pairs, expression *exp);
boxed_value *apply(List *env, List *tagbody_env_pairs, expression *func, List *args);

#endif

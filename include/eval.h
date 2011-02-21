#ifndef ALISP_EVAL
#define ALISP_EVAL

#include "expression.h"
#include "boxed_value.h"
#include "environment.h"
#include "hash.h"

boxed_value *eval(Env *env, expression *exp);
boxed_value *apply(Env *env, expression *func, List *args);

#endif

#ifndef ALISP_PRIMITIVES
#define ALISP_PRIMITIVES

#include "boxed_value.h"
#include "environment.h"
#include "expression.h"

boxed_value *apply_plus(List *values);
boxed_value *apply_times(List *values);
boxed_value *apply_minus(List *values);
boxed_value *apply_divide(List *values);
boxed_value *apply_if(Env *env, expression *predicate, expression *consequent, expression *alternative);
boxed_value *apply_eq(boxed_value *v1, boxed_value *v2);
boxed_value *apply_and(Env *env, List *exps);
boxed_value *apply_or(Env *env, List *exps);
boxed_value *apply_cons(boxed_value *v1, boxed_value *v2);
boxed_value *apply_car(boxed_value *v1);
boxed_value *apply_cdr(boxed_value *v1);
boxed_value *apply_progn(Env *env, List *exps);
boxed_value *apply_set(Env *env, expression *id, boxed_value *value);
#endif

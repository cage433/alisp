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
#endif

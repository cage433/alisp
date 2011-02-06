#ifndef ALISP_PRIMITIVES
#define ALISP_PRIMITIVES

#include "boxed_value.h"

boxed_value *apply_plus(List *values);
boxed_value *apply_times(List *values);
boxed_value *apply_minus(List *values);
boxed_value *apply_divide(List *values);

#endif

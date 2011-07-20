#ifndef ALISP_COMPILER
#define ALISP_COMPILER

#include "list.h"
#include "expression.h"
expression *process_expression(expression *exp);
List *compile(List *expressions);
expression *compile_expression(expression *exp);
#endif

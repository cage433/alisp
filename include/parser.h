#ifndef ALISP_PARSER
#define ALISP_PARSER

#include "stdio.h"
#include "list.h"
#include "expression.h"

List *parse_expressions(FILE *s);
expression *parse_expression_from_string(char *text);
List *parse_expressions_from_string(char *text);
#endif

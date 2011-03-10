#ifndef ALISP_PARSER
#define ALISP_PARSER

#include "stdio.h"
#include "list.h"
#include "expression.h"

List *parse_expressions(FILE *s, int do_compilation);
expression *parse_expression_from_string(char *text, int do_compilation);
List *parse_expressions_from_string(char *text, int do_compilation);
#endif

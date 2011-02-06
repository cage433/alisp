#ifndef ALISP_EXPRESSION
#define ALISP_EXPRESSION

#include "list.h"

typedef enum {
    exp_integer,
    exp_double,
    exp_identifier,
    exp_definition,
    exp_call
} expression_type;

struct expression;
struct expression_list;


typedef struct {
    char *name;
    List *args;                 // List of char*
    struct expression *body;
} definition_expression;

typedef struct {
    char *name;
    List *exps;
} call_expression;

typedef struct expression {
    expression_type type;
    union {
        int int_value;
        double double_value;
        char* identifier_value;
        definition_expression definition_value;
        call_expression call_value;
    };
} expression;

expression *make_integer_expression(int num);
expression *make_double_expression(double num);
expression *make_identifier_expression(char *identifier);
expression *make_call_expression(char *name, List *exps);
expression *make_definition_expression(char *name, List *args, expression *body);

int expressions_equal(const void *exp1, const void *exp2);
void print_expression(expression *exp);
#endif

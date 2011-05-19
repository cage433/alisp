#ifndef ALISP_EXPRESSION
#define ALISP_EXPRESSION

#include "list.h"
#include "hash.h"

typedef enum {
    exp_integer,
    exp_double,
    exp_identifier,
    exp_list        
} expression_type;


struct expression;


typedef struct expression {
    expression_type type;
    union {
        int int_value;
        double double_value;
        char* identifier_value;
        List *list_value;
    };
    char *text;
} expression;

expression *make_integer_expression(int num);
expression *make_double_expression(double num);
expression *make_identifier_expression(char *identifier);
expression *make_call_expression(struct expression *name, List *exps);
expression *make_definition_expression(char *name, struct expression *exp);
expression *make_function_expression(List *args, expression *body);
expression *make_progn_expression(List *exps);
expression *make_tagbody_expression(expression *whole_progn, Hash *tag_progn_map);
expression *make_list_expression(List *list);


int expressions_equal(const void *exp1, const void *exp2);
void print_expression(int depth, expression *exp);
char *expression_to_string(expression *exp);
int is_identifier(void *exp);

#endif

#ifndef ALISP_EXPRESSION
#define ALISP_EXPRESSION

#include "list.h"
#include "hash.h"

typedef enum {
    exp_integer,
    exp_double,
    exp_identifier,
    exp_definition,
    exp_call,
    exp_function,
    exp_progn,
    exp_tagbody
} expression_type;

struct expression;
struct expression_list;

typedef struct {
    List *args;                 // List of char*
    struct expression *body;
} function_expression;

typedef struct {
    char *name;
    struct expression *exp;
} definition_expression;

typedef struct {
    struct expression *func;
    List *exps;
} call_expression;

typedef struct {
    List *exps;
} progn_expression;

typedef struct {
    struct expression *whole_progn;
    Hash *tag_progn_map;
} tagbody_expression;

typedef struct expression {
    expression_type type;
    union {
        int int_value;
        double double_value;
        char* identifier_value;
        definition_expression definition_value;
        call_expression call_value;
        function_expression function_value;
        progn_expression progn_value;
        tagbody_expression tagbody_value;
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


int expressions_equal(const void *exp1, const void *exp2);
void print_expression(int depth, expression *exp);
char *expression_to_string(expression *exp);

#endif

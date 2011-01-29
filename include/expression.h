#ifndef ALISP_EXPRESSION
#define ALISP_EXPRESSION

typedef enum {
    exp_integer,
    exp_double,
    exp_identifier,
    exp_definition,
    exp_function_call,
    exp_operator_call
} expression_type;

struct expression;
struct expression_list;


typedef struct {
    char *name;
    int numargs;
    char **args;
    struct expression *body;
} definition_expression;

typedef struct {
    char *name;
    struct expression_list *exps;
} call_expression;

typedef struct expression {
    expression_type exp_type;
    union {
        int int_value;
        double double_value;
        char* identifier_value;
        definition_expression definition_value;
        call_expression call_value;
    };
} expression;

typedef struct expression_list{
        struct expression car;
        struct expression_list *cdr;
} expression_list;

#endif

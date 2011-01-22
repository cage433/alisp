typedef enum {
    exp_integer,
    exp_double,
    exp_identifier,
    exp_definition,
    exp_function_call,
    exp_operator_call
} expression_type;

struct expression;

typedef struct {
    char *name;
    char ** args;
    struct expression *exp;
} definition_expression;

typedef struct {
    char *name;
    struct expression **exps;
} function_call_expression;

typedef struct {
    char *name;
    struct expression **exps;
} operator_call_expression;

typedef struct {
    expression_type exp_type;
    union {
        int int_value;
        double double_value;
        char* identifier_value;
        definition_expression definition_value;
        function_call_expression function_call_value;
        operator_call_expression operator_call_value;
    };
} expression;




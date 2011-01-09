/*
        expression types

        1. Integer
        2. Double
        3. Identifier
        4. definition
        5. function call
        6. Operator call
*/
enum expression_type {
        exp_integer,
        exp_double,
        exp_identifier,
        exp_function_call,
        exp_function_definitiion
};
typedef enum expression_type expression_type;

typedef struct function_call function_call;
typedef struct function_definition function_definition;
struct expression {
        expression_type type;
        union {
                int int_value;
                double double_value;
                char *identifier_value;
                function_call *f_call;
                function_definition *f_definition;
        };
};

typedef struct expression expression;

struct expression_list {
    expression car;
    expression_list *cdr;
};

typedef struct expression_list expression_list;

struct function_call{
        char *name;
        int num_exps;
        struct expression* exps;
};

struct function_definition {
        char *name;
        int num_args;
        char **args;
        expression body;
};

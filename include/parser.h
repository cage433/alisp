/*
        Expression types

        1. Integer
        2. Double
        3. Identifier
        4. Definition
        5. Function call
        6. Operator call
*/
typedef struct Function_Call Function_Call;
typedef struct Function_Definition Function_Definition;
typedef struct Expression Expression;
struct Expression {
        int type;
        union {
                int int_value;
                double double_value;
                char *identifier_value;
                Function_Call *f_call;
                Function_Definition *f_definition;
        };
};

struct Function_Call{
        char *name;
        int num_exps;
        Expression* exps;
};

struct Function_Definition {
        char *name;
        int num_args;
        char **args;
        Expression body;
};

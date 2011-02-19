#include "expression.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "utils.h"


expression *make_integer_expression(int num){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_integer;
    exp->int_value = num;
    return exp;
}

expression *make_double_expression(double num){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_double;
    exp->double_value = num;
    return exp;
}

expression *make_identifier_expression(char *identifier){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_identifier;
    exp->identifier_value = strdup(identifier);
    return exp;
}

expression *make_call_expression(expression *func, List *exps){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_call;
    exp->call_value.func = func;
    exp->call_value.exps = exps;
    return exp;
}

expression *make_definition_expression(char *name, List *args, expression *body){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_definition;
    exp->definition_value.name = name;
    exp->definition_value.function.args = args;
    exp->definition_value.function.body = body;
    return exp;
}

expression *make_function_expression(List *args, expression *body){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_function;
    exp->function_value.args = args;
    exp->function_value.body = body;
    return exp;
}

int expressions_equal(const void *e1, const void *e2){
    expression *exp1 = (expression *)e1;
    expression *exp2 = (expression *)e2;
    if (exp1->type != exp2->type)
        return 0;
    else if (exp1->type == exp_integer)
        return exp1->int_value == exp2->int_value;
    else if (exp1->type == exp_double)
        return exp1->double_value == exp2->double_value;
    else if (exp1->type == exp_identifier)
        return strcmp(exp1->identifier_value, exp2->identifier_value) == 0;
    else if (exp1->type == exp_call){
        int exps_same = lists_equal(exp1->call_value.exps, exp2->call_value.exps, expressions_equal);
        return exps_same;
    } else if (exp1->type == exp_definition){
        int name_same = (strcmp(exp1->definition_value.name, exp2->definition_value.name) == 0);
        int args_same = lists_equal(exp1->definition_value.function.args, exp2->definition_value.function.args, strings_equal);
        int body_same = expressions_equal(exp1->definition_value.function.body, exp2->definition_value.function.body);
        return name_same && args_same && body_same;
    } else {
        printf("File %s, line %d\n", __FILE__, __LINE__); 
        printf("Unimplemented expressions_equal\n");
        exit(-1);
    }
}

void print_expression(expression *exp){
    if (exp->type == exp_integer)
        printf("Integer Expression %d\n", exp->int_value);
    else if (exp->type == exp_double)
        printf("Double expression %.2f\n", exp->double_value);
    else if (exp->type == exp_call){
        printf("Call expression\n");
        list_for_each(exp->call_value.exps, (for_each_fn_ptr)print_expression);
    } else if (exp->type == exp_identifier){
        printf("Identifier expression %s\n", exp->identifier_value);
    } else if (exp->type == exp_definition){
        printf("Definition expression%s\n", exp->definition_value.name);
    } else {
        printf("expression.c unimplemented\n");
        exit(-1);
    }
}


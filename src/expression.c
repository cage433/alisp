#include "expression.h"
#include "stdlib.h"
#include "string.h"


expression make_integer_expression(int num){
        expression exp;
        exp.exp_type = exp_integer;
        exp.int_value = num;
        return exp;
}

expression make_double_expression(double num){
        expression exp;
        exp.exp_type = exp_double;
        exp.double_value = num;
        return exp;
}

expression copy_double_expression(expression exp){
        return make_double_expression(exp.double_value);
}

expression make_identifier_expression(char *identifier){
        expression exp;
        exp.exp_type = exp_identifier;
        exp.identifier_value = calloc(1 + strlen(identifier), sizeof(char));
        strcpy(exp.identifier_value, identifier);
        return exp;
}

expression_list *cons_expression(expression elt, expression_list *list){
        expression_list *consed_list = (expression_list *)malloc(sizeof(expression_list));
        consed_list->car = elt;
        consed_list->cdr = list;
        return consed_list;
}

#include "expression.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"


expression *make_integer_expression(int num){
        expression *exp = (expression *)malloc(sizeof(expression));
        exp->type = exp_integer;
        exp->int_value = num;
        return exp;
}

expression *make_double_expression(double num){
        expression *exp = (expression *)malloc(sizeof(expression));
        exp->type = exp_double;
        exp->double_value = num;
        return exp;
}

expression make_identifier_expression(char *identifier){
        expression exp;
        exp.type = exp_identifier;
        exp.identifier_value = calloc(1 + strlen(identifier), sizeof(char));
        strcpy(exp.identifier_value, identifier);
        return exp;
}

int expressions_equal(expression *exp1, expression *exp2){
        if (exp1->type != exp2->type)
                return 0;
        else if (exp1->type == exp_integer)
                return exp1->int_value == exp2->int_value;
        else if (exp1->type == exp_double)
                return exp1->double_value == exp2->double_value;
        else {
                printf("File %s, line %d\n", __FILE__, __LINE__); 
                printf("Unimplemented\n");
                exit(-1);
        }
}

void print_expression(expression *exp){
        if (exp->type == exp_integer)
                printf("Integer Expression %d\n", exp->int_value);
        else {
                printf("expression.c unimplemented\n");
                exit(-1);
        }
}


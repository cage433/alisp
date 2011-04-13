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

expression *make_definition_expression(char *name, expression *definee){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_definition;
    exp->definition_value.name = name;
    exp->definition_value.exp = definee;
    return exp;
}

expression *make_function_expression(List *args, expression *body){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_function;
    exp->function_value.args = args;
    exp->function_value.body = body;
    return exp;
}

expression *make_progn_expression(List *exps){
    expression *exp = my_malloc(sizeof(expression));
    exp->type = exp_progn;
    exp->progn_value.exps = exps;
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
        int defined_exp_same = expressions_equal(exp1->definition_value.exp, exp2->definition_value.exp);
        return name_same && defined_exp_same;
    } else if (exp1->type == exp_function){
        int args_same = lists_equal(exp1->function_value.args, exp2->function_value.args, strings_equal);
        int body_same = expressions_equal(exp1->function_value.body, exp2->function_value.body);
        return args_same && body_same;
    } else {
        printf("File %s, line %d\n", __FILE__, __LINE__); 
        printf("Unimplemented expressions_equal\n");
        exit(-1);
    }
}

void print_expression(int depth, expression *exp){
    print_tabs(depth);
    if (exp->type == exp_integer)
        printf("Integer Expression %d\n", exp->int_value);
    else if (exp->type == exp_double)
        printf("Double expression %.2f\n", exp->double_value);
    else if (exp->type == exp_call){
        printf("Call expression\n");
        print_expression(depth + 1, exp->call_value.func);
        List *l = exp->call_value.exps;
        while (l != NULL){
            print_expression(depth + 1, (expression *)l->car);
            l = l->cdr;
        }
    } else if (exp->type == exp_identifier){
        printf("Identifier expression %s\n", exp->identifier_value);
    } else if (exp->type == exp_definition){
        printf("Definition expression%s\n", exp->definition_value.name);
    } else if (exp->type == exp_function){
        printf("Function expression\n");
        List *l = exp->function_value.args;
        while (l != NULL){
            print_tabs(depth + 1);
            printf("Arg %s\n", (char *)(l->car));
        }
        print_tabs(depth + 1);
        printf("Body\n");
        print_expression(depth + 1, exp->function_value.body);
    } else {
        printf("expression.c unimplemented\n");
        exit(-1);
    }
}

char *expression_to_string(expression *exp){
    char *buf = calloc(1000, sizeof(char));
    char *buf2 = calloc(1000, sizeof(char));
    List *l;
    void append(char *text){
        strcat(buf, text);
    }
    void append_func(function_expression func){
        append("(lambda (");
        l = func.args;
        while (l != NULL){
            append((char *)l->car);
            if (l->cdr != NULL)
                append(" ");
            l = l->cdr;
        }
        append(") ");
        append(expression_to_string(func.body));
        append(")");
    }

    if (exp->type == exp_integer){
        sprintf(buf2, "%d", exp->int_value);
        append(buf2);
    } else if (exp->type == exp_double){
        sprintf(buf2, "%.2f", exp->double_value);
        append(buf2);
    } else if (exp->type == exp_call){
        sprintf(buf2, "(");
        append(buf2);
        char *caller = expression_to_string(exp->call_value.func);
        append(caller);
        free(caller);
        l = exp->call_value.exps;
        while (l != NULL){
            append(" ");
            char *arg = expression_to_string((expression *)l->car);
            append(arg);
            free(arg);
            l = l->cdr;
        }
        append(")");
    } else if (exp->type == exp_identifier){
        sprintf(buf2, "%s", exp->identifier_value);
        append(buf2);
    } else if (exp->type == exp_definition){
        sprintf(buf2, "(def %s ", exp->definition_value.name);
        append(buf2);
        append(expression_to_string(exp->definition_value.exp));
        append(")");
    } else if (exp->type == exp_function){
        append_func(exp->function_value);
    } else {
        printf("expression.c unimplemented\n");
        exit(-1);
    }
    return strdup(buf);
}

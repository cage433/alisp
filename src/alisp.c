#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "environment.h"
#include "expression.h"
#include "eval.h"
#include "parser.h"
#include "boxed_value.h"

void repl(){
    char *buf = malloc(10000 * sizeof(char));
    char *exp_buf = malloc(10000 * sizeof(char));
    char *slashpos = NULL;
    expression *exp;
    List *env = create_env();
    printf("> ");
    while (1){
        fgets(buf, 10000, stdin);
        if ((slashpos = strchr(buf, '\\')) != NULL){
            strncat(exp_buf, buf, slashpos - buf);
            strcat(exp_buf, " ");
            printf("   ");
        } else {
            strcat(exp_buf, buf);
            exp = parse_expression_from_string(exp_buf);
            boxed_value *val = eval(env, NULL, exp);
            printf("  ");
            print_boxed_value(val, 0);
            strcpy(exp_buf, "");
            printf("> ");
        }
    }
}
int main(int ARGC, char **ARGV){
    die_unless(ARGC == 2, "Expect one argument");
    if (strcmp(ARGV[1], "-i") == 0)
        repl();
    else {
        FILE *f = fopen(ARGV[1], "r");
        List *exps = parse_expressions(f);
        List *env = create_env();
        while (exps != NULL){
            boxed_value *val = eval(env, NULL, exps->car);
            printf("  ");
            print_boxed_value(val, 0);
            exps = exps->cdr;
        }

    }

}

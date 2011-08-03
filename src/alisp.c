#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "environment.h"
#include "expression.h"
#include "eval.h"
#include "parser.h"
#include "boxed_value.h"
#include "unistd.h"

void repl(List *env){
    char *buf = malloc(10000 * sizeof(char));
    char *exp_buf = malloc(10000 * sizeof(char));
    char *slashpos = NULL;
    expression *exp;
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
            boxed_value *val = eval_exp(env, NULL, exp);
            printf("  ");
            print_boxed_value(val, 0);
            strcpy(exp_buf, "");
            printf("> ");
        }
    }
}
int main(int ARGC, char **ARGV){
    int c;
    char *file_to_load = NULL;
    int run_repl = 0;

    while((c = getopt(ARGC, ARGV, "if:")) != -1){
        switch(c){
            case 'f':
                file_to_load = malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(file_to_load, optarg);
                break;
            case 'i':
                run_repl = 1;
                break;
        }
    }
    List *env = create_env();
    if (file_to_load != NULL){
        FILE *f = fopen(file_to_load, "r");
        List *exps = parse_expressions(f);
        while (exps != NULL){
            boxed_value *val = eval_exp(env, NULL, exps->car);
            printf("  ");
            print_boxed_value(val, 0);
            exps = exps->cdr;
        }
    }

    if (run_repl)
        repl(env);
}

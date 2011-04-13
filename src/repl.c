#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "environment.h"
#include "expression.h"
#include "eval.h"

int main(){
    char *buf = malloc(10000 * sizeof(char));
    char *exp_buf = malloc(10000 * sizeof(char));
    char *slashpos = NULL;
    Env *env = create_env();
    printf("> ");
    while (1){
        fgets(buf, 10000, stdin);
        if ((slashpos = strchr(buf, '\\')) != NULL){
            strncat(exp_buf, buf, slashpos - buf);
            strcat(exp_buf, " ");
            printf("   ");
        } else {
            strcat(exp_buf, buf);
            expression *exp = parse_expression_from_string(exp_buf);
            boxed_value *val = eval(env, exp);
            printf("  ");
            print_boxed_value(val, 0);
            strcpy(exp_buf, "");
            printf("> ");
        }
    }
}

#include "environment.h"
#include "expression.h"
#include "boxed_value.h"
#include "parser.h"
#include "eval.h"

void main(){

    Env *env = create_env();
    expression *exp = parse_expression_from_string("(def factorial (n) (if (eq n 1) 1 (* (factorial (- n 1)) n)))");
    eval(env, exp);
    exp = parse_expression_from_string("(factorial 2000)");
    boxed_value *v = eval(env, exp);
    printf("Factorial 50 = ");
    print_boxed_value(v);
}

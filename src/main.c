#include "environment.h"
#include "expression.h"
#include "boxed_value.h"
#include "parser.h"
#include "eval.h"

void fn1(){
    Env *env = create_env();
    expression *exp = parse_expression_from_string("(def foo (x) (+ x 1))");
    eval(env, exp);
    exp = parse_expression_from_string("(def bar (x y) (+ x (foo y)))");
    eval(env, exp);
    exp = parse_expression_from_string("(bar 10 20)");
    boxed_value *v = eval(env, exp);
    print_boxed_value(v);
}
void fn2(){
    Env *env = create_env();
    expression *exp = make_integer_expression(12);
    boxed_value *v = eval(env, exp);
    print_boxed_value(v);
}

void main(){
    fn1();
    fn2();
    fn1();
    fn2();
}

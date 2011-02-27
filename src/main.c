#include "environment.h"
#include "expression.h"
#include "boxed_value.h"
#include "parser.h"
#include "eval.h"
#include "string.h"

void main(){
    char buf[1000];
    sprintf(buf, "hello ");
    strcat(buf,"world");
    printf("%s", buf);
    expression *exp = parse_expression_from_string("(def foo (x) (+ x 1))");
    printf("%s\n", expression_to_string(exp));
}

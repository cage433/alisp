#include "token.h"
#include "expression.h"

expression_list *parse_expressions(token_list *tokens){
        expression_list exps = NULL;
        if (tokens == NULL)
                return exps;
        else {
                switch (tokens->car.type){
                        case tok_identifier:
                                exps = cons_expression(identifier_expression(tokens->car.identifier_value));
                                break;
                        case tok_integer:
                                kkkkkk


                }
        }
}

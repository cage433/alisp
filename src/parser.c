#include "token.h"
#include "lexer.h"
#include "expression.h"
#include "list.h"
#include "stdio.h"
#include "stdlib.h"

expression *consume_integer_exp(List **tokens){
        int num = ((typed_token *)((**tokens).car))->intValue;
        *tokens = (*tokens)->cdr;
        expression *exp = make_integer_expression(num);
        return exp;
}
        
List *parse_expressions(FILE *stream){
        
        List *tokens = getTokens(stream);
        List *tokens2 = tokens;
        List *expressions = NULL;
        while (tokens2 != NULL){
                typed_token *tok = (typed_token *)tokens2->car;
                if (tok->type == tok_integer)
                        expressions = cons(consume_integer_exp(&tokens2), expressions);
                else {
                        printf("parser.c unimplemented\n");
                        exit(-1);
                }
        }
        List *result = reverse_list(expressions);
        free_list(expressions);
        free_tokens(tokens);
        free_list(tokens);
        return result;
}

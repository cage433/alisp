#include "token.h"
#include "lexer.h"
#include "expression.h"
#include "list.h"
#include "stdio.h"
#include "stdlib.h"

typed_token *token_car(List *tokens){
    return (typed_token *)(tokens->car);
}

expression *consume_integer_exp(List **tokens){
    typed_token *tok = token_car(*tokens);
    int num = tok->int_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_integer_expression(num);
    return exp;
}
    
expression *consume_double_exp(List **tokens){
    typed_token *tok = token_car(*tokens);
    double num = tok->double_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_double_expression(num);
    return exp;
}

expression *consume_identifier_exp(List **tokens){
    typed_token *tok = token_car(*tokens);
    char *identifier = tok->identifier_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_identifier_expression(identifier);
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
        else if (tok->type == tok_double)
            expressions = cons(consume_double_exp(&tokens2), expressions);
        else if (tok->type == tok_identifier)
            expressions = cons(consume_identifier_exp(&tokens2), expressions);
        else {
            printf("File %s, line %d\n", __FILE__, __LINE__); 
            printf("parser.c unimplemented for type %d\n", tok->type);
            exit(-1);
        }
    }
    List *result = reverse_list(expressions);
    free_list(expressions);
    free_tokens(tokens);
    free_list(tokens);
    return result;
}

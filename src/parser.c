#include "token.h"
#include "lexer.h"
#include "expression.h"
#include "list.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

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

void eat_left_paren(List **tokens){
    die_unless(token_car(*tokens)->type == tok_left_paren, "Expected '('");
    *tokens = (*tokens)->cdr;
}

void eat_right_paren(List **tokens){
    die_unless(token_car(*tokens)->type == tok_right_paren, "Expected ')'");
    *tokens = (*tokens)->cdr;
}
expression *consume_expression(List **tokens);
expression *consume_call_exp(List **tokens){
    eat_left_paren(tokens);
    char *name = consume_identifier_exp(tokens)->identifier_value;
    List *exps = NULL;
    while ((*tokens) != NULL && token_car(*tokens)->type != tok_right_paren){
        exps = cons(consume_expression(tokens), exps);
    }
    die_if(*tokens == NULL, "Expected right paren");
    eat_right_paren(tokens);

    List *exps_in_order = reverse_list(exps);
    free_list(exps);
    return make_call_expression(name, exps_in_order);
}

expression *consume_definition_exp(List **tokens){
    die("Not implemented\n");
}

expression *consume_expression(List **tokens){
    typed_token *tok = token_car(*tokens);
    if (tok->type == tok_integer)
        return consume_integer_exp(tokens);
    else if (tok->type == tok_double)
        return consume_double_exp(tokens);
    else if (tok->type == tok_identifier)
        return consume_identifier_exp(tokens);
    else if (tok->type == tok_left_paren){
        typed_token *nexttok = (typed_token *)nthelt(*tokens, 1);
        die_if((nexttok == NULL) || (nexttok->type != tok_identifier), "Expected idnetifier token after '('");
        if (strcmp(nexttok->identifier_value, "def") == 0)
            return consume_definition_exp(tokens);
        else
            return consume_call_exp(tokens);
    } else {
        printf("File %s, line %d\n", __FILE__, __LINE__); 
        printf("parser.c unimplemented for token type %d\n", tok->type);
        exit(-1);
    }
}


List *parse_expressions(FILE *stream){
    
    List *tokens = getTokens(stream);
    List *tokens2 = tokens;
    List *expressions = NULL;
    while (tokens2 != NULL){
        expressions = cons(consume_expression(&tokens2), expressions);
    }
    List *result = reverse_list(expressions);
    free_list(expressions);
    free_tokens(tokens);
    free_list(tokens);
    return result;
}

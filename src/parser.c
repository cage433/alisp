#include "token.h"
#include "lexer.h"
#include "expression.h"
#include "list.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "utils.h"
#include "compiler.h"


token *token_car(List *tokens){
    return (token *)(tokens->car);
}

void eat_left_paren(List **tokens){
    die_unless(token_car(*tokens)->type == tok_left_paren, "Expected '('");
    *tokens = (*tokens)->cdr;
}

void eat_right_paren(List **tokens){
    die_unless(token_car(*tokens)->type == tok_right_paren, "Expected ')'");
    *tokens = (*tokens)->cdr;
}

expression *consume_integer_exp(List **tokens){
    token *tok = token_car(*tokens);
    int num = tok->int_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_integer_expression(num);
    return exp;
}
    
expression *consume_double_exp(List **tokens){
    token *tok = token_car(*tokens);
    double num = tok->double_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_double_expression(num);
    return exp;
}

expression *consume_identifier_exp(List **tokens){
    token *tok = token_car(*tokens);
    char *identifier = tok->identifier_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_identifier_expression(identifier);
    return exp;
}

expression *consume_string_exp(List **tokens){
    token *tok = token_car(*tokens);
    char *text = tok->string_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_string_expression(text);
    return exp;
}

expression *consume_expression(List **tokens){
    token *tok = token_car(*tokens);
    if (tok->type == tok_integer)
        return consume_integer_exp(tokens);
    else if (tok->type == tok_double)
        return consume_double_exp(tokens);
    else if (tok->type == tok_identifier)
        return consume_identifier_exp(tokens);
    else if (tok->type == tok_string)
        return consume_string_exp(tokens);
    else if (tok->type == tok_left_paren){
        eat_left_paren(tokens);
        List *list = NULL;
        while (token_car(*tokens)->type != tok_right_paren){
            list = cons(consume_expression(tokens), list);
        }
        eat_right_paren(tokens);
        expression *exp = make_list_expression(reverse_list(list));
        free_list(list, nop_free_fn);
        return exp;
    }
}


List *parse_expressions(FILE *stream){
   List *tokens = getTokens(stream);
   List *tokens2 = tokens;
   List *expressions = NULL;
   while (tokens2 != NULL){
       expressions = cons(consume_expression(&tokens2), expressions);
   }
   List *uncompiled_expressions = reverse_list(expressions);
   free_list(expressions, nop_free_fn);
   free_list(tokens, (free_fn_ptr)free_token);
   return uncompiled_expressions;
}

List *parse_expressions_from_string(char *text){
    FILE *s = fmemopen(text, strlen(text), "r");
    List *l = parse_expressions(s);
    return l;
}

expression *parse_expression_from_string(char *text){
    List *l = parse_expressions_from_string(text);
    die_unless(l != NULL && l->cdr == NULL, "List should have a single element");
    expression *exp = l->car;
    my_free(l);
    return exp;
}

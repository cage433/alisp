#include "token.h"
#include "lexer.h"
#include "expression.h"
#include "list.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "utils.h"

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

int next_token_is_left_paren(List **tokens){
    return token_car(*tokens)->type == tok_left_paren;
}

int next_token_is_right_paren(List **tokens){
    return token_car(*tokens)->type == tok_right_paren;
}

void eat_left_paren(List **tokens){
    die_unless(token_car(*tokens)->type == tok_left_paren, "Expected '('");
    *tokens = (*tokens)->cdr;
}

void eat_right_paren(List **tokens){
    die_unless(token_car(*tokens)->type == tok_right_paren, "Expected ')'");
    *tokens = (*tokens)->cdr;
}

void eat_identifier(List **tokens, char *name){
    typed_token *car = token_car(*tokens);
    die_unless(car->type == tok_identifier && strcmp(car->identifier_value, name) == 0, "Expected identifier");
    *tokens = (*tokens)->cdr;
}

expression *consume_expression(List **tokens);

List *consume_upto_right_paren(List **tokens){
    List *exps = NULL;
    while ((*tokens) != NULL && token_car(*tokens)->type != tok_right_paren){
        exps = cons(consume_expression(tokens), exps);
    }
    die_if(*tokens == NULL, "Expected right paren");
    eat_right_paren(tokens);

    List *exps_in_order = reverse_list(exps);
    free_list(exps, nop_free_fn);
    return exps_in_order;
}

expression *construct_call_exp(List *exps){
    die_unless(listlen(exps) >= 1, "Require at least name");
    expression *first = (expression *)(exps->car);
    return make_call_expression(first, exps->cdr);
}

expression *consume_call_exp(List **tokens){
    eat_left_paren(tokens);
    List *exps = consume_upto_right_paren(tokens);
    return construct_call_exp(exps);
}

expression *consume_progn_expression(List **tokens){
    eat_left_paren(tokens);
    eat_identifier(tokens, "progn");
    List *exps = consume_upto_right_paren(tokens);
    expression *progn = make_progn_expression(exps);
    return progn;
}

/**
 * Three kinds of exp
 *  (def foo 3)
 *  (def foo (+ 1 2))
 *  (def foo (x) (+ 1 x))
 */
expression *consume_definition_exp(List **tokens){
    eat_left_paren(tokens);
    eat_identifier(tokens, "def");
    char *name = consume_identifier_exp(tokens)->identifier_value;
    if (next_token_is_left_paren(tokens)){
        eat_left_paren(tokens);
        List *exps = consume_upto_right_paren(tokens);
        if (next_token_is_right_paren(tokens)){
            // second case
            eat_right_paren(tokens);
            return make_definition_expression(name, construct_call_exp(exps));
        } else {
            // third case
            char *identifier_name(expression *exp){
                die_unless(exp->type == exp_identifier, "Expected identifier expression");
                return strdup(exp->identifier_value);
            }
            List *args = list_map(exps, (map_fn_ptr)identifier_name);
            expression *body = consume_expression(tokens);
            eat_right_paren(tokens);
            expression *function = make_function_expression(args, body);
            return make_definition_expression(name, function);
        }
    } else {
        // first case
        expression *exp = consume_expression(tokens);
        eat_right_paren(tokens);
        return make_definition_expression(name, exp);
    }
}


expression *consume_lambda_expression(List **tokens){
    eat_left_paren(tokens);
    eat_identifier(tokens, "lambda");
    eat_left_paren(tokens);
    List *arg_exps = consume_upto_right_paren(tokens);
    char *identifier_name(expression *exp){
        die_unless(exp->type == exp_identifier, "Expected identifier expression");
        return strdup(exp->identifier_value);
    }
    List *args = list_map(arg_exps, (map_fn_ptr)identifier_name);
    expression *body = consume_expression(tokens);
    eat_right_paren(tokens);
    return make_function_expression(args, body);
}

expression *consume_tagbody_expression(List **tokens){
    eat_left_paren(tokens);
    List *exps = NULL;
    while (!next_token_is_right_paren(tokens)){
        exps = cons(consume_expression(tokens), exps);
    }
    

    List *progn_exps = NULL;
    Hash *hash = hash_create_with_string_keys();

    while (exps != NULL){
        expression *exp = exps->car;
        if (exp->type == exp_identifier){
            hash_add(hash, exp->identifier_value, make_progn_expression(progn_exps));
        } else {
            progn_exps = cons(exp, progn_exps);
        }
        exps = exps->cdr;
    }

    eat_right_paren(tokens);

    expression *tagbody = make_tagbody_expression(
        make_progn_expression(progn_exps),
        hash
    );
    free_list(exps, nop_free_fn);
    return tagbody;
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
        if (nexttok == NULL)
            die("Expected more tokens");
        else if (nexttok->type == tok_identifier) {
            if (strings_equal(nexttok->identifier_value, "def"))
                return consume_definition_exp(tokens);
            else if (strings_equal(nexttok->identifier_value, "lambda"))
                return consume_lambda_expression(tokens);
            else if (strings_equal(nexttok->identifier_value, "progn"))
                return consume_progn_expression(tokens);
            else if (strings_equal(nexttok->identifier_value, "tagbody"))
                return consume_tagbody_expression(tokens);
            else
                return consume_call_exp(tokens);
        } else if (nexttok->type == tok_left_paren){
            return consume_call_exp(tokens);
        } else {
            printf("File %s, line %d\n", __FILE__, __LINE__); 
            printf("parser.c unimplemented for token type %d\n", tok->type);
            exit(-1);
        }
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
    free_list(expressions, nop_free_fn);
    free_list(tokens, free_token);
    return result;
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

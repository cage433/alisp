#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "token.h"
#include "utils.h"

token *integer_token(int num, char *text){
    token *tok = (token *)my_malloc(sizeof(token));
    tok->type = tok_integer;
    tok->int_value = num;
    tok->text = strdup(text);
    return tok;
}

token *double_token(double num, char *text){
    token *tok = (token *)my_malloc(sizeof(token));
    tok->type = tok_double;
    tok->double_value = num;
    tok->text = strdup(text);
    return tok;
}

token *identifier_token(char *identifier, char *text){
    token *tok = (token *)my_malloc(sizeof(token));
    tok->type = tok_identifier;
    tok->identifier_value = strdup(identifier);
    tok->text = strdup(text);
    return tok;
}

void printtoken(token *tok){
    printf("Token %s\n", tok->text);
}

int tokens_equal(const void *t1, const void *t2){
    token *tok1 = (token *)t1;
    token *tok2 = (token *)t2;
    switch(tok1->type){
        case tok_left_paren:
            return tok2->type == tok_left_paren;
        case tok_right_paren:
            return tok2->type == tok_right_paren;
        case tok_integer:
            if (tok2->type == tok_integer)
                return tok1->int_value == tok2->int_value;
            else
                return 0;
        case tok_double:
            if (tok2->type == tok_double)
                return tok1->double_value == tok2->double_value;
            else
                return 0;
        case tok_identifier:
            if (tok2->type == tok_identifier)
                return strcmp(tok1->identifier_value, tok2->identifier_value) == 0;
            else
                return 0;
    }
}

void free_token(token *token){
    switch (token->type){
        case tok_left_paren:
        case tok_right_paren:
            break;
        case tok_identifier:
            my_free(token->identifier_value);
            my_free(token);
            break;
        case tok_double:
        case tok_integer:
            my_free(token);
            break;
        default:
            die("Unrecognised token");
    }
}

    

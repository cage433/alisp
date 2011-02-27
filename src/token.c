#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "token.h"
#include "utils.h"

typed_token *integer_token(int num, char *text){
    typed_token *tok = (typed_token *)my_malloc(sizeof(typed_token));
    tok->type = tok_integer;
    tok->int_value = num;
    tok->text = strdup(text);
    return tok;
}

typed_token *double_token(double num, char *text){
    typed_token *tok = (typed_token *)my_malloc(sizeof(typed_token));
    tok->type = tok_double;
    tok->double_value = num;
    tok->text = strdup(text);
    return tok;
}

typed_token *identifier_token(char *identifier, char *text){
    typed_token *tok = (typed_token *)my_malloc(sizeof(typed_token));
    tok->type = tok_identifier;
    tok->identifier_value = strdup(identifier);
    tok->text = strdup(text);
    return tok;
}

void printtoken(typed_token *tok){
    printf("Token %s\n", tok->text);
}

int tokens_equal(const void *t1, const void *t2){
    typed_token *tok1 = (typed_token *)t1;
    typed_token *tok2 = (typed_token *)t2;
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

void free_token(void *token){
    typed_token *tok = token;
    switch (tok->type){
        case tok_left_paren:
        case tok_right_paren:
            break;
        case tok_identifier:
            my_free(tok->identifier_value);
            my_free(tok);
            break;
        case tok_double:
        case tok_integer:
            my_free(tok);
            break;
        default:
            die("Unrecognised token");
    }
}

    

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "token.h"

typed_token *integer_token(int num){
    typed_token *tok = (typed_token *)malloc(sizeof(typed_token));
    tok->type = tok_integer;
    tok->int_value = num;
    return tok;
}

typed_token *double_token(double num){
    typed_token *tok = (typed_token *)malloc(sizeof(typed_token));
    tok->type = tok_double;
    tok->double_value = num;
    return tok;
}

typed_token *identifier_token(char *identifier){
    typed_token *tok = (typed_token *)malloc(sizeof(typed_token));
    tok->type = tok_identifier;
    tok->identifier_value = strdup(identifier);
    return tok;
}

void printtoken(typed_token *tok){
    switch(tok->type){
        case tok_left_paren:
            printf("LEFT PAREN\n");
            break;
        case tok_right_paren:
            printf("RIGHT PAREN\n");
            break;
        case tok_integer:
            printf("INTEGER %d\n", tok->int_value);
            break;
        case tok_double:
            printf("DOUBLE %G\n", tok->double_value);
            break;
        case tok_identifier:
            printf("IDENTIFIER %s\n", tok->identifier_value);
            break;
        default:
            printf("Unexpected token\n");
            exit(1);
    }
}

void print_token_list(List *toks){
    int i = 0;
    while (toks != NULL){
        printf("Token %d\n", i++);
        printtoken(toks->car);
        toks = toks->cdr;
    }
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

typed_token *copy_token(typed_token tok){
    if (tok.type == tok_identifier){
        return identifier_token(tok.identifier_value);
    } else {
        typed_token *copy = (typed_token *)malloc(sizeof(typed_token));
        memcpy(copy, &tok, sizeof(typed_token));
        return copy;
    }
}

void free_token(void *token){
    typed_token *tok = token;
    switch (tok->type){
        case tok_left_paren:
        case tok_right_paren:
            break;
        case tok_identifier:
            free(tok->identifier_value);
            free(tok);
            break;
        case tok_double:
        case tok_integer:
            free(tok);
            break;
        default:
            die("Unrecognised token");
    }
}

    

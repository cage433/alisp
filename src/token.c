#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "stdarg.h"

#include "token.h"


void printtoken(typed_token tok){
        switch(tok.type){
                case tok_left_paren:
                        printf("LEFT PAREN\n");
                        break;
                case tok_right_paren:
                        printf("RIGHT PAREN\n");
                        break;
                case tok_integer:
                        printf("INTEGER %d\n", tok.intValue);
                        break;
                case tok_double:
                        printf("DOUBLE %G\n", tok.doubleValue);
                        break;
                case tok_identifier:
                        printf("IDENTIFIER %s\n", tok.identifierValue);
                        break;
                default:
                        printf("Unexpected token\n");
                        exit(1);
        }
}

void print_token_list(token_list *toks){
        int i = 0;
        while (toks != NULL){
                printf("Token %d\n", i++);
                printtoken(toks->car);
                toks = toks->cdr;
        }
}

int tokens_equal(typed_token tok1, typed_token tok2){
        switch(tok1.type){
                case tok_left_paren:
                        return tok2.type == tok_left_paren;
                case tok_right_paren:
                        return tok2.type == tok_right_paren;
                case tok_integer:
                        if (tok2.type == tok_integer)
                                return tok1.intValue == tok2.intValue;
                        else
                                return 0;
                case tok_double:
                        if (tok2.type == tok_double)
                                return tok1.doubleValue == tok2.doubleValue;
                        else
                                return 0;
                case tok_identifier:
                        if (tok2.type == tok_identifier)
                                return strcmp(tok1.identifierValue, tok2.identifierValue) == 0;
                        else
                                return 0;
        }
}

typed_token copy_token(typed_token tok){
        if (tok.type == tok_identifier){
                int len = strlen(tok.identifierValue);
                char *newIdentifierValue = (char *)calloc(len + 1, sizeof(char));
                strcpy(newIdentifierValue, tok.identifierValue);
                typed_token result;
                result.type = tok_identifier;
                result.identifierValue = newIdentifierValue;
                return result;
        } else
                return tok;
}


int token_lists_equal(token_list *l1, token_list *l2){
        while(l1 != NULL || l2 != NULL){
                if (l1 == NULL || l2 == NULL){
                        return 0;
                }
                else if (tokens_equal(l1->car, l2->car)){
                        l1 = l1->cdr;
                        l2 = l2->cdr;
                } else {
                        return 0;
                }
        }
        return 1;
}
                


token_list *cons(typed_token elt, token_list *list){
        token_list *consed_list = (token_list *)malloc(sizeof(token_list));
        consed_list->car = copy_token(elt);
        consed_list->cdr = list;
        return consed_list;
}

token_list *reverse_token_list(token_list *list){
        token_list *reverse = NULL;
        while (list != NULL){
                reverse = cons(copy_token(list->car), reverse);
                list = list->cdr;
        }
        return reverse;
}

void free_token(typed_token tok){
        if (tok.type == tok_identifier)
                free(tok.identifierValue);
}
void free_token_list(token_list *list){
        while (list != NULL){
                free_token(list->car);
                token_list *cdr = list->cdr;
                free(list);
                list = cdr;
        }
}
token_list *make_token_list(int size, ...){
        va_list(ap);
        va_start(ap, size);
        token_list *list = NULL;
        int i;
        for (i = 0; i < size; ++i){
                typed_token elt = va_arg(ap, typed_token);
                list = cons(elt, list);
        }
        token_list *result = reverse_token_list(list);
        free_token_list(list);
        return result;
}

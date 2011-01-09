#include "parser.h"
#include "token.h"
#include <stdio.h>
#include "string.h"
#include "stdlib.h"

expression identifier_expression(char *identifier){
        char *identifier_copy = (char *)calloc(strlen(identifier) + 1, sizeof(char));
        strcpy(identifier_copy, identifier);
        expression exp;
        exp.type = exp_identifier;
        exp.identifier_value = identifier_copy;
        return exp;
}

expression int_expression(int num){
        expression exp;
        exp.type = exp_integer;
        exp.int_value = num;
        return exp;
}

expression double_expression(double num){
        expression exp;
        exp.type = exp_double;
        exp.double_value = num;
        return exp;
}

expression parse_paren_expression(token_list **tokens){
        expression exp;
        return exp;
}

expression parse_expression(token_list **tokens){
        token_list *list = *tokens;
        typed_token first_token = (**tokens).car;
        switch (first_token.type){
                case tok_left_paren:
                        return parse_paren_expression(tokens);
                case tok_identifier:
                        (*tokens)++;
                        return identifier_expression(first_token.identifierValue);
                case tok_integer:
                        (*tokens)++;
                        return int_expression(first_token.intValue);
                case tok_double:
                        (*tokens)++;
                        return double_expression(first_token.doubleValue);
                default:
                        printf("Unexpected token\n");
                        exit(1);
        }
}

#ifndef ALISP_INCLUDE
#define ALISP_INCLUDE

#include "stdio.h"

enum token_type {
        tok_left_paren,
        tok_identifier,
        tok_integer,
        tok_double,
        tok_right_paren
};

struct typed_token{
        enum token_type type;
        union {
                int intValue;
                double doubleValue;
                char* identifierValue;
        };
};

struct token_list{
        struct typed_token *car;
        struct token_list *cdr;
};

static struct typed_token LEFT_PAREN = {tok_left_paren, 0};
static struct typed_token RIGHT_PAREN = {tok_right_paren, 0};

struct token_list *getTokens(FILE *stream);
int tokens_equal(struct typed_token *tok1, struct typed_token *tok2);

#endif



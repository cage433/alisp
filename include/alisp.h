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

typedef struct {
        enum token_type type;
        union {
                int intValue;
                double doubleValue;
                char* identifierValue;
        };
} typed_token;

struct token_list{
        typed_token *car;
        struct token_list *cdr;
};

static typed_token LEFT_PAREN = {tok_left_paren, 0};
static typed_token RIGHT_PAREN = {tok_right_paren, 0};

struct token_list *getTokens(FILE *stream);
int tokens_equal(typed_token *tok1, typed_token *tok2);

#endif



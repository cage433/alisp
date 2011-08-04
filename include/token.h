#ifndef ALISP_TOKEN
#define ALISP_TOKEN

#include "list.h"

typedef enum {
    tok_left_paren,
    tok_identifier,
    tok_integer,
    tok_double,
    tok_right_paren,
    tok_string
} token_type;

typedef struct {
    token_type type;
    union {
        int int_value;
        double double_value;
        char *identifier_value;
        char *string_value;
    };
    char *text;
} token;

token *integer_token(int num, char *text);
token *double_token(double num, char *text);
token *identifier_token(char *identifier, char *text);
token *string_token(char *text);

static token LEFT_PAREN = {tok_left_paren, 0, "("};
static token RIGHT_PAREN = {tok_right_paren, 0, ")"};

int tokens_equal(const void *tok1, const void *tok2);
void free_token(token *token);

#endif



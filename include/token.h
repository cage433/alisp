#ifndef TOKEN_INCLUDE
#define TOKEN_INCLUDE

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

typedef struct token_list{
        typed_token car;
        struct token_list *cdr;
} token_list;

static typed_token LEFT_PAREN = {tok_left_paren, 0};
static typed_token RIGHT_PAREN = {tok_right_paren, 0};

int tokens_equal(typed_token tok1, typed_token tok2);
token_list *cons(typed_token elt, token_list *list);

token_list *reverse_token_list(token_list *list);

void free_token_list(token_list *list);
token_list *make_token_list(int size, ...);
#endif


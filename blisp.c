#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"

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

void printtoken(struct typed_token *tok){
        switch(tok->type){
                case tok_left_paren:
                        printf("LEFT PAREN\n");
                        break;
                case tok_right_paren:
                        printf("RIGHT PAREN\n");
                        break;
                case tok_integer:
                        printf("INTEGER %d\n", tok->intValue);
                        break;
                case tok_double:
                        printf("DOUBLE %G\n", tok->doubleValue);
                        break;
                case tok_identifier:
                        printf("IDENTIFIER %s\n", tok->identifierValue);
                        break;
        }
}


struct token_list{
        struct typed_token *car;
        struct token_list *cdr;
};

struct token_list *cons(struct typed_token *elt, struct token_list *list){
        struct token_list *consed_list = (struct token_list *)malloc(sizeof(struct token_list));
        consed_list->car = elt;
        consed_list->cdr = list;
        return consed_list;
}

struct typed_token LEFT_PAREN = {tok_left_paren, 0};
struct typed_token RIGHT_PAREN = {tok_right_paren, 0};

struct char_buffer{
        char *array;
        int max_arr_length;
        int string_length;
};

void increase_arr_size(struct char_buffer *fac){
        char *new_array = (char *)calloc(2 * fac->max_arr_length, sizeof(char));
        strcpy(new_array, fac->array);
        free(fac->array);
        fac->array = new_array;
        fac->max_arr_length = 2 * fac->max_arr_length;
}

void clear_char_buffer(struct char_buffer * buf){
        free(buf->array);
        buf->array = (char *)calloc(10, sizeof(char));
        buf->max_arr_length = 10;
        buf ->string_length = 0;
}

void add_char(char ch, struct char_buffer *fac){
        if (fac->string_length + 1 >= fac->max_arr_length){
                increase_arr_size(fac);
        }
        fac->array[fac->string_length] = ch;
        ++(fac->string_length);
}

struct typed_token *consume_identifier(struct char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isspace(ch) || ch == ')'){
                        if (ch == ')'){
                                ungetc(ch, stream);
                        }
                        char *new_array = (char *)calloc(buf->string_length + 1, sizeof(char));
                        strcpy(new_array, buf->array);

                        struct typed_token *tok = (struct typed_token *)malloc(sizeof(struct typed_token));
                        tok->type = tok_identifier;
                        tok->identifierValue = new_array;
                        return tok;
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                }
        }
}

struct typed_token *consume_double(struct char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isdigit(ch)){
                        add_char(ch, buf);
                } else if (isspace(ch) || ch == ')'){
                        if (ch == ')'){
                                ungetc(ch, stream);
                        }
                        double num = atof(buf->array);
                        struct typed_token *tok = (struct typed_token *)malloc(sizeof(struct typed_token));
                        tok->type = tok_double;
                        tok->doubleValue = num;
                        return tok;
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                        return consume_identifier(buf, stream);
                }
        }
}

struct typed_token *consume_integer(struct char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isdigit(ch)){
                        add_char(ch, buf);
                } else if (ch == '.'){
                        add_char(ch, buf);
                        return consume_double(buf, stream);
                } else if (isspace(ch) || ch == ')'){
                        if (ch == ')'){
                                ungetc(ch, stream);
                        }
                        int num = atoi(buf->array);
                        struct typed_token *tok = (struct typed_token *)malloc(sizeof(struct typed_token));
                        tok->type = tok_integer;
                        tok->intValue = num;
                        return tok;
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                        return consume_identifier(buf, stream);
                }
        }
}


struct token_list *getTokens(FILE *stream){
        struct token_list *x = NULL;
        struct char_buffer buf = {(char *) calloc(10, sizeof(char)), 10, 0};
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (ch == '(')
                        x = cons(&LEFT_PAREN, x);
                else if (ch == ')')
                        x = cons(&RIGHT_PAREN, x);
                else if (isdigit(ch)){
                        add_char(ch, &buf);
                        x = cons(consume_integer(&buf, stream), x);
                } else if (isspace(ch))
                {}
                else {
                        add_char(ch, &buf);
                        x = cons(consume_identifier(&buf, stream), x);
                }
                clear_char_buffer(&buf);
        }
        return x;
}

int main(int argc, char *argv[]){
        printf("First arg is %s\n", argv[1]);
        FILE *stream = fopen(argv[1], "r");
        int ch;

        struct token_list *toks = getTokens(stream);
        while (toks != NULL){
                printtoken(toks->car);
                toks = toks->cdr;
        }
        return 0;
}

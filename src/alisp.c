#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "stdarg.h"

#include "token.h"
#include "alisp.h"

typedef struct char_buffer{
        char *array;
        int max_arr_length;
        int string_length;
} char_buffer;


void increase_arr_size(char_buffer *fac){
        char *new_array = (char *)calloc(2 * fac->max_arr_length, sizeof(char));
        strcpy(new_array, fac->array);
        free(fac->array);
        fac->array = new_array;
        fac->max_arr_length = 2 * fac->max_arr_length;
}

void clear_char_buffer(char_buffer * buf){
        free(buf->array);
        buf->array = (char *)calloc(10, sizeof(char));
        buf->max_arr_length = 10;
        buf->string_length = 0;
}

void add_char(char ch, char_buffer *fac){
        if (fac->string_length + 1 >= fac->max_arr_length){
                increase_arr_size(fac);
        }
        fac->array[fac->string_length] = ch;
        ++(fac->string_length);
}

typed_token make_identifier_token(char_buffer *buf){
        char *new_array = (char *)calloc(buf->string_length + 1, sizeof(char));
        strcpy(new_array, buf->array);
        typed_token result;
        result.type = tok_identifier;
        result.identifierValue = new_array;
        return result;
}
typed_token consume_identifier(char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isspace(ch) || ch == ')'){
                        if (ch == ')'){
                                ungetc(ch, stream);
                        }
                        return make_identifier_token(buf);
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                }
        }
        return make_identifier_token(buf);
}

typed_token make_double_token(char_buffer *buf){
        int num = atof(buf->array);
        typed_token result;
        result.type = tok_double;
        result.intValue = num;
        return result;
}
typed_token consume_double(char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isdigit(ch)){
                        add_char(ch, buf);
                } else if (isspace(ch) || ch == ')'){
                        if (ch == ')'){
                                ungetc(ch, stream);
                        }
                        return make_double_token(buf);
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                        return consume_identifier(buf, stream);
                }
        }
        return make_double_token(buf);
}

typed_token make_integer_token(char_buffer *buf){
        int num = atoi(buf->array);
        typed_token result;
        result.type = tok_integer;
        result.intValue = num;
        return result;
}

typed_token consume_integer(char_buffer *buf, FILE *stream){
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
                        return make_integer_token(buf);
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                        return consume_identifier(buf, stream);
                }
        }
        return make_integer_token(buf);
}


token_list *getTokens(FILE *stream){
        token_list *x = NULL;
        char_buffer buf = {(char *) calloc(10, sizeof(char)), 10, 0};
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (ch == '(')
                        x = cons(LEFT_PAREN, x);
                else if (ch == ')')
                        x = cons(RIGHT_PAREN, x);
                else if (isdigit(ch)){
                        add_char(ch, &buf);
                        typed_token tok = consume_integer(&buf, stream);
                        x = cons(tok, x);
                } else if (isspace(ch))
                {}
                else {
                        add_char(ch, &buf);
                        x = cons(consume_identifier(&buf, stream), x);
                }
                clear_char_buffer(&buf);
        }
        token_list *result = reverse_token_list(x);
        free_token_list(x);
        return result;
}


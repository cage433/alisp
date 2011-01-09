#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "stdarg.h"

#include "token.h"
#include "lexer.h"

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

int isparen(char ch){
        return ch == '(' || ch == ')';
}

typed_token consume_identifier(char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isspace(ch) || isparen(ch)){
                        if (isparen(ch)){
                                ungetc(ch, stream);
                        }
                        return identifier_token(buf->array);
                } else {
                        add_char(ch, buf);
                }
        }
        return identifier_token(buf->array);
}

typed_token consume_double(char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isdigit(ch)){
                        add_char(ch, buf);
                } else if (isspace(ch) || isparen(ch)){
                        if (isparen(ch)){
                                ungetc(ch, stream);
                        }
                        return double_token(atof(buf->array));
                } else {
                        add_char(ch, buf);
                        return consume_identifier(buf, stream);
                }
        }
        return double_token(atof(buf->array));
}

typed_token consume_integer(char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isdigit(ch)){
                        add_char(ch, buf);
                } else if (ch == '.'){
                        add_char(ch, buf);
                        return consume_double(buf, stream);
                } else if (isspace(ch) || isparen(ch)){
                        if (isparen(ch)){
                                ungetc(ch, stream);
                        }
                        return integer_token(atoi(buf->array));
                } else {
                        add_char(ch, buf);
                        return consume_identifier(buf, stream);
                }
        }
        return integer_token(atoi(buf->array));
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


#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "stdarg.h"

#include "alisp.h"

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
                if (l1 == NULL || l2 == NULL)
                        return 0;
                else if (tokens_equal(l1->car, l2->car)){
                        l1 = l1->cdr;
                        l2 = l2->cdr;
                } else
                        return 0;
        }
        return 1;
}
                


token_list *cons(typed_token elt, token_list *list){
        token_list *consed_list = (token_list *)malloc(sizeof(token_list));
        consed_list->car = copy_token(elt);
        consed_list->cdr = list;
        return consed_list;
}

typedef struct char_buffer{
        char *array;
        int max_arr_length;
        int string_length;
} char_buffer;


token_list *make_token_list(int size, ...){
        va_list(ap);
        va_start(ap, size);
        token_list *list = NULL;
        int i;
        for (i = 0; i < size; ++i){
                typed_token elt = va_arg(ap, typed_token);
                list = cons(elt, list);
        }
        return list;
}

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

typed_token consume_identifier(char_buffer *buf, FILE *stream){
        int ch;
        while ((ch = getc(stream)) != EOF){
                if (isspace(ch) || ch == ')'){
                        if (ch == ')'){
                                ungetc(ch, stream);
                        }
                        char *new_array = (char *)calloc(buf->string_length + 1, sizeof(char));
                        strcpy(new_array, buf->array);
                        typed_token result;
                        result.type = tok_identifier;
                        result.identifierValue = new_array;
                        return result;
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                }
        }
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
                        double num = atof(buf->array);
                        typed_token result;
                        result.type = tok_double;
                        result.doubleValue = num;
                        return result;
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                        return consume_identifier(buf, stream);
                }
        }
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
                        int num = atoi(buf->array);
                        typed_token result;
                        result.type = tok_integer;
                        result.intValue = num;
                        return result;
                } else if (ch == '(') {
                        printf("Unexpected left paren ");
                        exit(1);
                } else {
                        add_char(ch, buf);
                        return consume_identifier(buf, stream);
                }
        }
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

//int main(int argc, char *argv[]){
//        printf("First arg is %s\n", argv[1]);
//        FILE *stream = fopen(argv[1], "r");
//        int ch;
//
//        token_list *toks = getTokens(stream);
//        while (toks != NULL){
//                printtoken(toks->car);
//                toks = toks->cdr;
//        }
//        return 0;
//}

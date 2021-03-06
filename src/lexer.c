#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "stdarg.h"

#include "token.h"
#include "lexer.h"
#include "list.h"
#include "utils.h"


int isparen(char ch){
    return ch == '(' || ch == ')';
}

token_type consume_identifier(FILE *buf, FILE *stream){
    int ch;
    while ((ch = getc(stream)) != EOF){
        if (isspace(ch) || isparen(ch)){
            if (isparen(ch)){
            ungetc(ch, stream);
            }
            return tok_identifier;
        } else {
            putc(ch, buf);
        }
    }
    return tok_identifier;
}

void consume_string(FILE *buf, FILE *stream){
    int ch;
    while ((ch = getc(stream)) != '"')
        putc(ch, buf);
}

token_type consume_double(FILE *buf, FILE *stream){
    int ch;
    while ((ch = getc(stream)) != EOF){
        if (isdigit(ch)){
            putc(ch, buf);
        } else if (isspace(ch) || isparen(ch)){
            if (isparen(ch)){
            ungetc(ch, stream);
            }
            return tok_double;
        } else {
            putc(ch, buf);
            return consume_identifier(buf, stream);
        }
    }
    return tok_double;
}

token_type consume_integer(FILE *buf, FILE *stream){
    int ch;
    while ((ch = getc(stream)) != EOF){
        if (isdigit(ch)){
            putc(ch, buf);
        } else if (ch == '.'){
            putc(ch, buf);
            return consume_double(buf, stream);
        } else if (isspace(ch) || isparen(ch)){
            if (isparen(ch)){
                ungetc(ch, stream);
            }
            return tok_integer;
        } else {
            putc(ch, buf);
            return consume_identifier(buf, stream);
        }
    }
    return tok_integer;
}

token_type consume_hash_symbol(FILE *buf, FILE *stream){
    int ch;
    ch = getc(stream);
    if (ch == '\\'){
        ch = getc(stream);
        putc(ch, buf);
        return tok_char;
    } else {
        die("Unexpected character");
    }
}

token *consume_token(FILE *stream){
    // We know here that stream is not at EOF, 
    // nor is its next character whitespace
    token *tok;
    int ch;
    ch = getc(stream);
    if (ch == '(')
        tok = &LEFT_PAREN;
    else if (ch == ')')
        tok = &RIGHT_PAREN;
    else {
        char *p;
        size_t s;
        FILE *buf = open_memstream(&p, &s);
        int token_type;
        if (isdigit(ch)){
            putc(ch, buf);
            token_type = consume_integer(buf, stream);
        } else if (ch == '"'){
            token_type = tok_string;
            consume_string(buf, stream);
        } else if (ch == '#'){
            token_type = consume_hash_symbol(buf, stream);
        } else {
            putc(ch, buf);
            token_type = consume_identifier(buf, stream);
        }
        fflush(buf);
        switch (token_type){
            case tok_identifier:
                tok = identifier_token(p, p);
                break;
            case tok_double:
                tok = double_token(atof(p), p);
                break;
            case tok_integer:
                tok = integer_token(atoi(p), p);
                break;
            case tok_string:
                tok = string_token(p);
                break;
            case tok_char:
                tok = char_token(*p);
                break;
        }
        fclose(buf);
        my_free(p);
    }
    return tok;
}



List *getTokens(FILE *stream){
    List *x = NULL;
    int ch;
    while ((ch = getc(stream)) != EOF){
        if (isspace(ch)){
        } else {
            ungetc(ch, stream);
            token *tok = consume_token(stream);
            x = cons(tok, x);
        }
    }
    List *result = reverse_list(x);
    free_list(x, nop_free_fn);
    return result;
}



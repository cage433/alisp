#ifndef ALISP_BOXED_VALUE
#define ALISP_BOXED_VALUE

#include "stdlib.h"
#include "stdio.h"
#include "expression.h"
#include "hash.h"
#include "environment.h"

typedef enum {
    boxed_int,
    boxed_double,
    boxed_string,
    boxed_closure,
    boxed_cons,
    boxed_nil,
    boxed_expression,
    boxed_stream,
    boxed_char
} boxed_value_type;

struct boxed_value;
struct boxed_cons{
    struct boxed_value *car;
    struct boxed_value *cdr;
};

struct boxed_closure{
    Hash *frame;
    function_expression function;
};

typedef struct boxed_value{
    boxed_value_type type;
    int ref_count;
    union {
        int int_value;
        double double_value;
        char *string_value;
        struct boxed_closure closure_value;
        struct boxed_cons cons_value;
        expression *expression_value;
        FILE *stream_value;
        char char_value;
    };
} boxed_value;

int boxed_values_equal(const void *box1, const void *box2);
boxed_value *make_boxed_int(int num);
boxed_value *make_boxed_double(double num);
boxed_value *make_boxed_string(char *str);
boxed_value *make_boxed_stream(FILE *stream);
boxed_value *make_boxed_cons(boxed_value *car, boxed_value *cdr);
boxed_value *make_boxed_expression(expression *exp);
boxed_value *make_boxed_char(char c);
extern boxed_value *NIL;
extern boxed_value *TRUE;
void print_boxed_value(boxed_value *v, int indent);
void inc_ref_count(boxed_value *v);
void dec_ref_count(boxed_value *v);
boxed_value *make_boxed_closure(List *env, function_expression def);

#endif

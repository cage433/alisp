#ifndef ALISP_BOXED_VALUE
#define ALISP_BOXED_VALUE
#include "stdlib.h"
#include "expression.h"

typedef enum {
    boxed_int,
    boxed_double,
    boxed_string,
    boxed_function,
    boxed_cons,
    boxed_nil
} boxed_value_type;

struct boxed_value;
struct boxed_cons{
    struct boxed_value *car;
    struct boxed_value *cdr;
} ;

typedef struct boxed_value{
    boxed_value_type type;
    int ref_count;
    union {
        int int_value;
        double double_value;
        char *string_value;
        function_expression function_value;
        struct boxed_cons cons_value;
    };
} boxed_value;

int boxed_values_equal(const void *box1, const void *box2);
boxed_value *make_boxed_int(int num);
boxed_value *make_boxed_double(double num);
boxed_value *make_boxed_string(char *str);
boxed_value *make_boxed_function(function_expression def);
boxed_value *make_boxed_cons(boxed_value *car, boxed_value *cdr);
extern boxed_value *NIL;
extern boxed_value *TRUE;
void print_boxed_value(boxed_value *v);
void inc_ref_count(boxed_value *v);
void dec_ref_count(boxed_value *v);

#endif

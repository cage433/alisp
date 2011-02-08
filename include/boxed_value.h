#ifndef ALISP_BOXED_VALUE
#define ALISP_BOXED_VALUE
#include "stdlib.h"
#include "expression.h"

typedef enum {
    boxed_int,
    boxed_double,
    boxed_string,
    boxed_definition,
    boxed_list
} boxed_value_type;

typedef struct {
    boxed_value_type type;
    union {
        int int_value;
        double double_value;
        char *string_value;
        definition_expression definition_value;
        List *list_value;
    };
} boxed_value;

int boxed_values_equal(const void *box1, const void *box2);
boxed_value *make_boxed_int(int num);
boxed_value *make_boxed_double(double num);
boxed_value *make_boxed_string(char *str);
boxed_value *make_boxed_definition(definition_expression def);
boxed_value *make_boxed_list(List *list);
static boxed_value NIL_VALUE = {boxed_list, 0};
static boxed_value *NIL = &NIL_VALUE;
static boxed_value TRUE_VALUE = {boxed_int, 77};
static boxed_value *TRUE = &TRUE_VALUE;
void free_boxed_value(boxed_value *b);
void print_boxed_value(boxed_value *v);
extern int first_ptr;

#endif

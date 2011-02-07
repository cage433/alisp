#ifndef ALISP_BOXED_VALUE
#define ALISP_BOXED_VALUE

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
void free_boxed_value(boxed_value *b);
void print_boxed_value(boxed_value *v);

#endif

#include "stdlib.h"

#include "primitives.h"
#include "boxed_value.h"

double as_double_value(boxed_value *value){
    if (value->type == boxed_int)
        return (double)(value->int_value);
    else if (value->type == boxed_double)
        return value->double_value;
    else
        die("Boxed value not int or double");
}


boxed_value *apply_plus(List *values){
    boxed_value *acc = make_boxed_int(0);
    while (values != NULL){
        boxed_value *next = values->car;
        if (acc->type == boxed_int && next->type == boxed_int){
            acc->int_value += next->int_value;
        } else {
            double acc_double_value = as_double_value(acc);
            double next_double_value = as_double_value(next);
            acc->type = boxed_double;
            acc->double_value = acc_double_value + next_double_value;
        }
        values = values->cdr;
    }
    return acc;
}



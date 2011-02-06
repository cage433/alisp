#include "stdlib.h"

#include "primitives.h"
#include "boxed_value.h"
#include "list.h"

double as_double_value(boxed_value *value){
    if (value->type == boxed_int)
        return (double)(value->int_value);
    else if (value->type == boxed_double)
        return value->double_value;
    else
        die("Boxed value not int or double");
}

boxed_value *apply_plus(List *values){
    void *add_values(boxed_value *bv1, boxed_value *bv2){
        boxed_value *result;
        if (bv1->type == boxed_int && bv2->type == boxed_int)
            result = make_boxed_int(bv1->int_value + bv2->int_value);
        else
            result = make_boxed_double(as_double_value(bv1) + as_double_value(bv2));
        free(bv1);
        return result;
    }

    return list_fold(values, make_boxed_int(0), (fold_fn_ptr)add_values);
}

boxed_value *apply_times(List *values){
    void *mult_values(boxed_value *bv1, boxed_value *bv2){
        boxed_value *result;
        if (bv1->type == boxed_int && bv2->type == boxed_int)
            result = make_boxed_int(bv1->int_value * bv2->int_value);
        else
            result = make_boxed_double(as_double_value(bv1) * as_double_value(bv2));
        free(bv1);
        return result;
    }

    return list_fold(values, make_boxed_int(1), (fold_fn_ptr)mult_values);
}

boxed_value *apply_minus(List *values){
    void *minus_values(boxed_value *bv1, boxed_value *bv2){
        boxed_value *result;
        if (bv1->type == boxed_int && bv2->type == boxed_int)
            result = make_boxed_int(bv1->int_value - bv2->int_value);
        else
            result = make_boxed_double(as_double_value(bv1) - as_double_value(bv2));
        free(bv1);
        return result;
    }
    if (values == NULL)
        return make_boxed_int(0);
    else
        return list_fold(values->cdr, values->car, (fold_fn_ptr)minus_values);
}

boxed_value *apply_divide(List *values){
    void *divide_values(boxed_value *bv1, boxed_value *bv2){
        boxed_value *result;
        if (bv1->type == boxed_int && bv2->type == boxed_int){
            int n1 = bv1->int_value;
            int n2 = bv2->int_value;
            div_t d = div(n1, n2);
            if (d.rem == 0)
                result = make_boxed_int(d.quot);
            else
                result = make_boxed_double(as_double_value(bv1) / as_double_value(bv2));
        } else
            result = make_boxed_double(as_double_value(bv1) / as_double_value(bv2));
        free(bv1);
        return result;
    }
    if (values == NULL)
        return make_boxed_int(1);
    else
        return list_fold(values->cdr, values->car, (fold_fn_ptr)divide_values);
}

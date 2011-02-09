#include "stdlib.h"

#include "primitives.h"
#include "boxed_value.h"
#include "list.h"
#include "hash.h"
#include "eval.h"
#include "stdio.h"

double as_double_value(boxed_value *value){
    if (value->type == boxed_int)
        return (double)(value->int_value);
    else if (value->type == boxed_double)
        return value->double_value;
    else {
        printf("Unexpected value ");
        print_boxed_value(value);
        die("Boxed value not int or double");
    }
}

boxed_value *copy_boxed_number(boxed_value *v){
    if (v->type == boxed_int)
        return make_boxed_int(v->int_value);
    else if (v->type == boxed_double)
        return make_boxed_double(v->double_value);
    else
        die("Only do subtracton on ints and doubles");
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
    else {
        return list_fold(values->cdr, copy_boxed_number(values->car), (fold_fn_ptr)minus_values);
    }
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
        return list_fold(values->cdr, copy_boxed_number(values->car), (fold_fn_ptr)divide_values);
}

boxed_value *apply_if(Env *env, expression *predicate, expression *consequent, expression *alternative){
    boxed_value *predicate_value = eval(env, predicate);
    if (boxed_values_equal(predicate_value, NIL))
        return eval(env, alternative);
    else
        return eval(env, consequent);
}

boxed_value *apply_eq(boxed_value *v1, boxed_value *v2){
    if (boxed_values_equal(v1, v2))
        return TRUE;
    else {
        return NIL;
    }
}

// TODO - Deal with the memory leak 
boxed_value *apply_and(Env *env, List *exps){
    boxed_value *result = TRUE;
    while (exps != NULL){
        boxed_value *v = eval(env, exps->car);
        if (boxed_values_equal(v, NIL)){
            return NIL;
        }
        exps = exps->cdr;
        if (exps == NULL)
            result = v;
    }
    return result;
}

boxed_value *apply_or(Env *env, List *exps){
    while (exps != NULL){
        boxed_value *v = eval(env, exps->car);
        if (!boxed_values_equal(v, NIL))
            return v;
        exps = exps->cdr;
    }
    return NIL;
}

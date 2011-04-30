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
        print_boxed_value(value, 0);
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
        my_free(bv1);
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
        my_free(bv1);
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
        my_free(bv1);
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
        my_free(bv1);
        return result;
    }
    if (values == NULL)
        return make_boxed_int(1);
    else
        return list_fold(values->cdr, copy_boxed_number(values->car), (fold_fn_ptr)divide_values);
}

boxed_value *apply_if(List *env, List *tagbody_env_pairs, List *expressions){
    int num_exps = listlen(expressions);
    die_if(num_exps < 2 || num_exps > 3, "if expressions need 2 or 3 sub expressions");
    boxed_value *predicate_value = eval(env, tagbody_env_pairs, expressions->car);
    if (boxed_values_equal(predicate_value, NIL))
        if (num_exps == 2)
            return NIL;
        else
            return eval(env, tagbody_env_pairs, nthelt(expressions, 2));
    else
        return eval(env, tagbody_env_pairs, nthelt(expressions, 1));
}

boxed_value *apply_set(List *env, expression *id, boxed_value *value){
    die_unless(id->type == exp_identifier, "first argument must be an identifier");
    set_value_in_env(env, id->identifier_value, value);
    return value;
}

boxed_value *apply_eq(boxed_value *v1, boxed_value *v2){
    if (boxed_values_equal(v1, v2))
        return TRUE;
    else {
        return NIL;
    }
}

boxed_value *apply_numeric_comparator(char* comp, List *values){
    double asDouble(boxed_value *v){
        if (v->type == boxed_int)
            return 0.0 + v->int_value;
        else if (v->type == boxed_double)
            return v->double_value;
        else
            die("non numeric comparator");
    }

    boxed_value *compare (boxed_value *v1, boxed_value *v2){
        double d1 = asDouble(v1);
        double d2 = asDouble(v2);

        if (strings_equal(comp, "<"))
            return d1 < d2 ? TRUE : NIL;
        else if (strings_equal(comp, ">"))
            return d1 > d2 ? TRUE : NIL;
        else if (strings_equal(comp, "<="))
            return d1 <= d2 ? TRUE : NIL;
        else if (strings_equal(comp, ">="))
            return d1 >= d2 ? TRUE : NIL;
        else
            die(make_msg("unexpected comparator %s", comp));
    }
    while (values->cdr != NULL){
        boxed_value *comp = compare(values->car, values->cdr->car);
        if (boxed_values_equal(comp, NIL))
            return comp;
        values = values->cdr;
    }
    return TRUE;
}

// TODO - Deal with the memory leak 
boxed_value *apply_and(List *env, List *tagbody_env_pairs, List *exps){
    boxed_value *result = TRUE;
    while (exps != NULL){
        boxed_value *v = eval(env, tagbody_env_pairs, exps->car);
        if (boxed_values_equal(v, NIL)){
            return NIL;
        }
        exps = exps->cdr;
        if (exps == NULL)
            result = v;
    }
    return result;
}

// TODO - Deal with the memory leak 
boxed_value *apply_or(List *env, List *tagbody_env_pairs, List *exps){
    while (exps != NULL){
        boxed_value *v = eval(env, tagbody_env_pairs, exps->car);
        if (!boxed_values_equal(v, NIL))
            return v;
        exps = exps->cdr;
    }
    return NIL;
}

boxed_value *apply_cons(boxed_value *v1, boxed_value *v2){
    return make_boxed_cons(v1, v2);
}

boxed_value *apply_car(boxed_value *v1){
    die_unless(v1->type == boxed_cons, "can only get car of a cons");
    return v1->cons_value.car;
}

boxed_value *apply_cdr(boxed_value *v1){
    die_unless(v1->type == boxed_cons, "can only get cdr of a cons");
    return v1->cons_value.cdr;
}

boxed_value *apply_progn(List *env, List *tagbody_env_pairs, List *exps){
    boxed_value *result = NIL;
    while (exps != NULL){
        result = eval(env, tagbody_env_pairs, exps->car);
        exps = exps->cdr;
    }
    return result;
}


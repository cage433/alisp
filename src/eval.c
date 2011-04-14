#include "eval.h"
#include "utils.h"
#include "environment.h"
#include "expression.h"
#include "primitives.h"
#include "boxed_value.h"
#include "frame.h"

char *PRIMITIVES[12] = {"+", "*", "-", "/", "cons", "car", "cdr", "eq", "if", "and", "or", "set!"};
int is_primitive(char *identifier){
    int i;
    for (i = 0; i < 12; ++i)
        if (strings_equal(PRIMITIVES[i], identifier))
            return 1;
    return 0;
}
    
static int indent = 0;
boxed_value *eval(Env *env, expression *exp){
    definition_expression def;
    call_expression call;
    boxed_value *value;
    List *exps;
    switch(exp->type){
        case exp_integer:
            value = make_boxed_int(exp->int_value);
            break;
        case exp_double:
            value = make_boxed_double(exp->double_value);
            break;
        case exp_identifier:
            value = env_lookup(env, exp->identifier_value);
            break;
        case exp_definition:
            def = exp->definition_value;
            if (env_has_binding(env, def.name)){
                char *buf = malloc(1000 * sizeof(char));
                snprintf(buf, 1000, "Already have binding for %s", def.name);
                die(buf);
            }
            if (def.exp->type == exp_function){
                boxed_value *boxed_fun = make_boxed_closure(create_env(), def.exp->function_value);
                value = boxed_fun;
            } else {
                value = eval(env, def.exp);
            }
            define_value_in_env(env, def.name, value);
            break;
        case exp_call:
            call = exp->call_value;
            value = apply(env, call.func, call.exps);
            break;
        case exp_function:
            value = make_boxed_closure(env, exp->function_value);
            break;
        case exp_progn:
            exps = exp->progn_value.exps;
            while (exps != NULL){
                value = eval(env, exps->car);
                exps = exps->cdr;
            }
            break;
        default:
            die("Unexpected expression type");
    }
    indent -=1;
    return value;
}

boxed_value *apply_primitive(Env *env, char *op_name, List *arg_exps){
    boxed_value *value;
    boxed_value *eval_exp(expression *exp){
        return eval(env, exp);
    }
    if (strings_equal(op_name, "if")){
        die_unless(listlen(arg_exps) == 3, "If requires three arguments exactly");
        value =  apply_if(env, nthelt(arg_exps, 0), nthelt(arg_exps, 1), nthelt(arg_exps, 2));
    } else if (strings_equal(op_name, "and")){
        value = apply_and(env, arg_exps);
    } else if (strings_equal(op_name, "or")){
        value = apply_or(env, arg_exps);
    } else if (strings_equal(op_name, "set!")){
        die_unless(listlen(arg_exps) == 2, "set! requires one identifier and one variable");
        expression *id = nthelt(arg_exps, 0);
        value = eval_exp(nthelt(arg_exps, 1));
        apply_set(env, id, value);
    } else {
        List *arg_values = list_map(arg_exps, (map_fn_ptr)eval_exp);
        if (strings_equal(op_name, "eq")){
            die_unless(listlen(arg_values) == 2, "Eq requires two arguments exactly");
            value = apply_eq(nthelt(arg_values, 0), nthelt(arg_values, 1));
        } else if (strcmp(op_name, "+") == 0)
            value = apply_plus(arg_values);
        else if (strcmp(op_name, "*") == 0)
            value = apply_times(arg_values);
        else if (strcmp(op_name, "-") == 0)
           value = apply_minus(arg_values);
        else if (strcmp(op_name, "/") == 0)
            value = apply_divide(arg_values);
        else if (strcmp(op_name, "cons") == 0){
            die_unless(listlen(arg_values) == 2, "Cons requires two values");
            value = apply_cons(nthelt(arg_values, 0), nthelt(arg_values, 1));
        } else if (strcmp(op_name, "car") == 0){
            die_unless(listlen(arg_values) == 1, "car requires one value");
            value = apply_car(nthelt(arg_values, 0));
        } else if (strcmp(op_name, "cdr") == 0) {
            die_unless(listlen(arg_values) == 1, "cdr requires one value");
            value = apply_cdr(nthelt(arg_values, 0));
        } else {
            die("Unexpected primitive");
        }
    }
    return value;
}
boxed_value *apply(Env *env, expression *func_exp, List *arg_exps){
    boxed_value *value;

    if (func_exp->type == exp_identifier && is_primitive(func_exp->identifier_value))
        value = apply_primitive(env, func_exp->identifier_value, arg_exps);
    else {
        boxed_value *func = eval(env, func_exp);
        boxed_value *eval_exp(expression *exp){
            return eval(env, exp);
        }
        List *arg_values = list_map(arg_exps, (map_fn_ptr)eval_exp);

        Hash *frame = frame_create(func->closure_value.function.args, arg_values);
        env_add_frame(env, func->closure_value.frame);
        env_add_frame(env, frame);
        value = eval(env, func->closure_value.function.body);
        env_drop_frame(env, 1);
        env_drop_frame(env, 0);
    }
    indent -=1;
    return value;
}

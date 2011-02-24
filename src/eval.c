#include "eval.h"
#include "utils.h"
#include "environment.h"
#include "expression.h"
#include "primitives.h"
#include "boxed_value.h"

char *PRIMITIVES[11] = {"+", "*", "-", "/", "cons", "car", "cdr", "eq", "if", "and", "or"};
int is_primitive(char *identifier){
    int i;
    for (i = 0; i < 11; ++i)
        if (strings_equal(PRIMITIVES[i], identifier))
            return 1;
    return 0;
}
    
boxed_value *eval(Env *env, expression *exp){
    definition_expression def;
    call_expression call;
    boxed_value *value;
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
            boxed_value *boxed_fun = make_boxed_closure(create_env(), def.function);
            hash_add(env->base, def.name, boxed_fun);
            value = boxed_fun;
            break;
        case exp_call:
            call = exp->call_value;
            value = apply(env, call.func, call.exps);
            break;
        case exp_function:
            value = make_boxed_closure(create_env(), exp->function_value);
            break;
        default:
            die("Unexpected expression type");
    }
    return value;
}

boxed_value *apply_primitive(Env *env, char *op_name, List *arg_exps){
    boxed_value *value;
    if (strings_equal(op_name, "if")){
        die_unless(listlen(arg_exps) == 3, "If requires three arguments exactly");
        value =  apply_if(env, nthelt(arg_exps, 0), nthelt(arg_exps, 1), nthelt(arg_exps, 2));
    } else if (strings_equal(op_name, "and")){
        value = apply_and(env, arg_exps);
    } else if (strings_equal(op_name, "or")){
        value = apply_or(env, arg_exps);
    } else {
        boxed_value *eval_exp(expression *exp){
            return eval(env, exp);
        }
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
        }
        else if (strcmp(op_name, "car") == 0){
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
    if (func_exp->type == exp_identifier && is_primitive(func_exp->identifier_value))
        return apply_primitive(env, func_exp->identifier_value, arg_exps);
    else {
        boxed_value *func = eval(env, func_exp);
        boxed_value *eval_exp(expression *exp){
            return eval(env, exp);
        }
        env_add_frame(env, func->closure_value.frame);
        void print_key_value(char *key){
            printf("Key = %s, value = ", key);
            print_boxed_value((boxed_value *)(hash_value(func->closure_value.frame, key)));
        }
        list_for_each(hash_keys(func->closure_value.frame), (for_each_fn_ptr)print_key_value);
        printf("\n\n");
        List *arg_values = list_map(arg_exps, (map_fn_ptr)eval_exp);
        env_drop_frame(env, 0);

        Hash *frame = frame_create(func->closure_value.function.args, arg_values);
        env_add_frame(env, frame);
        boxed_value *value = eval(env, func->closure_value.function.body);
        env_drop_frame(env, 1);
        return value;
    }
}

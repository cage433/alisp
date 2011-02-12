#include "eval.h"
#include "utils.h"
#include "environment.h"
#include "expression.h"
#include "primitives.h"

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
            boxed_value *boxed_def = make_boxed_definition(def);
            hash_add(env->base, def.name, boxed_def);
            value = boxed_def;
            break;
        case exp_call:
            call = exp->call_value;
            value = apply(env, call.name, call.exps);
            break;
        default:
            die("Unexpected expression type");
    }
    return value;
}

boxed_value *apply(Env *env, char *op_name, List *arg_exps){
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
            boxed_value *boxed_def = env_lookup(env, op_name);
            die_unless(boxed_def->type == boxed_definition, "Can only apply functions\n");
            definition_expression def = boxed_def->definition_value;
            Hash *frame = frame_create(def.args, arg_values);

            env_add_frame(env, frame);
            value = eval(env, def.body);
            env_drop_frame(env);
        }
    }
    return value;
}

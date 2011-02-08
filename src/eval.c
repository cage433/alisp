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
List *eval_expressions(Env *env, List *expressions){
    List *l = NULL;
    while (expressions != NULL){
        l = cons(eval(env, expressions->car), l);
        expressions = expressions->cdr;
    }
    List *result = reverse_list(l);
    free_list(l, nop_free_fn);
    return result;
}


boxed_value *apply(Env *env, char *op_name, List *arg_exps){
    boxed_value *value;
    if (strings_equal(op_name, "if")){
        die_unless(listlen(arg_exps) == 3, "If requires three arguments exactly");
        value =  apply_if(env, nthelt(arg_exps, 0), nthelt(arg_exps, 1), nthelt(arg_exps, 2));
    } else {
        List *arg_values = eval_expressions(env, arg_exps);
        if (strings_equal(op_name, "eq")){
            die_unless(listlen(arg_values) == 2, "Eq requires two arguments exactly");
            value = apply_eq(nthelt(arg_values, 0), nthelt(arg_values, 1));
        } else if (strcmp(op_name, "+") == 0)
            value = apply_plus(arg_values);
        else if (strcmp(op_name, "*") == 0)
            value = apply_times(arg_values);
        else if (strcmp(op_name, "-") == 0){
            value = apply_minus(arg_values);
        }else if (strcmp(op_name, "/") == 0)
            value = apply_divide(arg_values);
        else {
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

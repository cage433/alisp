#include "eval.h"
#include "utils.h"
#include "environment.h"
#include "expression.h"
#include "primitives.h"

boxed_value *eval(Env *env, expression *exp){
    definition_expression def;
    call_expression call;
    boxed_value *eval_exp(expression *arg){
        return eval(env, arg);
    }
    switch(exp->type){
        case exp_integer:
            return make_boxed_int(exp->int_value);
        case exp_double:
            return make_boxed_double(exp->double_value);
        case exp_identifier:
            return env_lookup(env, exp->identifier_value);
        case exp_definition:
            def = exp->definition_value;
            boxed_value *boxed_def = make_boxed_definition(def);
            hash_add(env->base, def.name, boxed_def);
            return boxed_def;
        case exp_call:
            call = exp->call_value;
            List *arg_values = list_map(call.exps, (map_fn_ptr)eval_exp);
            return apply(env, call.name, arg_values);
        default:
            die("Unexpected expression type");
    }

}

boxed_value *apply(Env *env, char *op_name, List *args){
    if (strcmp(op_name, "+") == 0)
        return apply_plus(args);
    else if (strcmp(op_name, "*") == 0)
        return apply_times(args);
    else if (strcmp(op_name, "-") == 0)
        return apply_minus(args);
    else if (strcmp(op_name, "/") == 0)
        return apply_divide(args);
    else {
        boxed_value *boxed_def = env_lookup(env, op_name);
        die_unless(boxed_def->type == boxed_definition, "Can only apply functions\n");
        definition_expression def = boxed_def->definition_value;
        Hash *frame = frame_create(def.args, args);
        env_add_frame(env, frame);
        boxed_value *value = eval(env, def.body);
        env_drop_frame(env);
        return value;
    }
}

#include "eval.h"
#include "utils.h"
#include "environment.h"
#include "expression.h"
#include "primitives.h"
#include "boxed_value.h"
#include "frame.h"
#include "pair.h"

char *PRIMITIVES[17] = {
    "+", "*", "-", "/", "cons", 
    "car", "cdr", "eq", "if", 
    "and", "or", "set!", "go",
    "<", ">", "<=", ">="
};
int is_primitive(char *identifier){
    int i;
    for (i = 0; i < 17; ++i)
        if (strings_equal(PRIMITIVES[i], identifier))
            return 1;
    return 0;
}

char *COMPARATORS[4] = {
    "<", ">", "<=", ">="
};
int is_comparator(char *identifier){
    int i;
    for (i = 0; i < 4; ++i)
        if (strings_equal(COMPARATORS[i], identifier))
            return 1;
    return 0;
}
    
static int indent = 0;

List *free_innermost_tagbody(List *tagbody_env_pairs){
    die_if(tagbody_env_pairs == NULL, "Tagbody list is empty");
    List *cdr = tagbody_env_pairs->cdr;
    free(tagbody_env_pairs->car);
    free(tagbody_env_pairs);
    return cdr;
}

// Tagbodies always returb NIL, hence the void
// return type
void eval_tagbody(List *env, List *tagbody_pairs, expression *tagbody_exp){
    tagbody_pairs = cons(make_pair(env, tagbody_exp->tagbody_value.tag_progn_map), tagbody_pairs);
    eval(env, tagbody_pairs, tagbody_exp->tagbody_value.whole_progn);
    list_drop(tagbody_pairs);
}

boxed_value *eval(List *env, List* tagbody_env_pairs, expression *exp){
    definition_expression def;
    call_expression call;
    boxed_value *value;
    List *exps;
    switch(exp->type){
        case exp_tagbody:
            eval_tagbody(env, tagbody_env_pairs, exp);
            value = NIL;
            break;
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
            if (env_has_binding(env, def.name))
                die(make_msg("Already have binding for %s", def.name));

            if (def.exp->type == exp_function){
                boxed_value *boxed_fun = make_boxed_closure(create_env(), def.exp->function_value);
                value = boxed_fun;
            } else {
                value = eval(env, tagbody_env_pairs, def.exp);
            }
            define_value_in_env(env, def.name, value);
            break;
        case exp_call:
            call = exp->call_value;
            value = apply(env, tagbody_env_pairs, call.func, call.exps);
            break;
        case exp_function:
            value = make_boxed_closure(env, exp->function_value);
            break;
        case exp_progn:
            exps = exp->progn_value.exps;
            while (exps != NULL){
                value = eval(env, tagbody_env_pairs, exps->car);
                exps = exps->cdr;
            }
            break;
        default:
            die(make_msg("Unexpected expression type %d", exp->type));
    }
    indent -=1;
    return value;
}

boxed_value *apply_primitive(List *env, List *tagbody_env_pairs, char *op_name, List *arg_exps){
    boxed_value *value;
    boxed_value *eval_exp(expression *exp){
        return eval(env, tagbody_env_pairs, exp);
    }
    int op_name_equals(char *name){
        return strings_equal(op_name, name);
    }

    if (op_name_equals("if")){
        value = apply_if(env, tagbody_env_pairs, arg_exps);
    } else if (op_name_equals("and")){
        value = apply_and(env, tagbody_env_pairs, arg_exps);
    } else if (op_name_equals("or")){
        value = apply_or(env, tagbody_env_pairs, arg_exps);
    } else if (op_name_equals("set!")){
        die_unless(listlen(arg_exps) == 2, "set! requires one identifier and one variable");
        expression *id = nthelt(arg_exps, 0);
        value = eval_exp(nthelt(arg_exps, 1));
        apply_set(env, id, value);
    } else if (op_name_equals("go")) {
            die_unless(listlen(arg_exps) == 1, "go requires one value");

            char *label = ((expression *)(arg_exps->car))->identifier_value;
            pair *p;
            Hash *h;
            while (tagbody_env_pairs != NULL){
                p = tagbody_env_pairs->car;
                h = (Hash *)(p->rhs);
                if (hash_contains(h, label)){
                    break;
                } else {
                    tagbody_env_pairs = list_drop(tagbody_env_pairs);
                }
            }
            die_if(tagbody_env_pairs == NULL, make_msg("Can't find label %s", label));
            List *tagbody_env = (List *)(p->lhs);
            while (env != NULL && env != tagbody_env){
                if (env == tagbody_env)
                    break;
                die_if(env == NULL, "Env should have contained tagbody env");
                env = env_drop_frame(env);
            }
            value = eval(tagbody_env, tagbody_env_pairs, (expression *)(hash_value(h, label)));
    } else {
        List *arg_values = list_map(arg_exps, (map_fn_ptr)eval_exp);
        if (op_name_equals("eq")){
            die_unless(listlen(arg_values) == 2, "Eq requires two arguments exactly");
            value = apply_eq(nthelt(arg_values, 0), nthelt(arg_values, 1));
        } else if (op_name_equals("+"))
            value = apply_plus(arg_values);
        else if (op_name_equals("*"))
            value = apply_times(arg_values);
        else if (op_name_equals("-"))
           value = apply_minus(arg_values);
        else if (op_name_equals("/"))
            value = apply_divide(arg_values);
        else if (op_name_equals("cons")){
            die_unless(listlen(arg_values) == 2, "Cons requires two values");
            value = apply_cons(nthelt(arg_values, 0), nthelt(arg_values, 1));
        } else if (op_name_equals("car")){
            die_unless(listlen(arg_values) == 1, "car requires one value");
            value = apply_car(nthelt(arg_values, 0));
        } else if (op_name_equals("cdr")) {
            die_unless(listlen(arg_values) == 1, "cdr requires one value");
            value = apply_cdr(nthelt(arg_values, 0));
        } else if (is_comparator(op_name)){
            die_unless(listlen(arg_values) >= 2, "numeric comparisons require at least two values");
            value = apply_numeric_comparator(op_name, arg_values);
        } else {
            die("Unexpected primitive");
        }
    }
    return value;
}
boxed_value *apply(List *env, List *tagbody_env_pairs, expression *func_exp, List *arg_exps){
    boxed_value *value;

    if (func_exp->type == exp_identifier && is_primitive(func_exp->identifier_value))
        value = apply_primitive(env, tagbody_env_pairs, func_exp->identifier_value, arg_exps);
    else {
        boxed_value *func = eval(env, tagbody_env_pairs, func_exp);
        boxed_value *eval_exp(expression *exp){
            return eval(env, tagbody_env_pairs, exp);
        }
        List *arg_values = list_map(arg_exps, (map_fn_ptr)eval_exp);

        env = env_add_frame(env, copy_frame(func->closure_value.frame));
        Hash *frame = frame_create(func->closure_value.function.args, arg_values);
        env = env_add_frame(env, frame);
        value = eval(env, tagbody_env_pairs, func->closure_value.function.body);
        env = env_drop_frame(env);
        env = env_drop_frame(env);
    }
    indent -=1;
    return value;
}

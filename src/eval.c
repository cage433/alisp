#include "eval.h"
#include "utils.h"
#include "environment.h"
#include "expression.h"
#include "primitives.h"
#include "boxed_value.h"
#include "compiler.h"
#include "frame.h"
#include "pair.h"
#include "utils.h"


char *PRIMITIVES[19] = {
    "+", "*", "-", "/", "cons", 
    "car", "cdr", "eq", "if", 
    "and", "or", "set!", "go",
    "<", ">", "<=", ">=", "quote",
    "list"
};
int is_primitive(char *identifier){
    int i;
    for (i = 0; i < 19; ++i)
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

jmp_buf buf;
boxed_value *eval_exp_handling_exception(List *env, List* tagbody_env_pairs, expression *exp){
    if (!setjmp(buf)){
        return eval_exp(env, tagbody_env_pairs, exp);
    } else {
        printf("An error occured\n");
        return NIL;
    }
}
boxed_value *eval_exp(List *env, List* tagbody_env_pairs, expression *exp){
    return eval_boxed(env, tagbody_env_pairs, make_boxed_expression(exp));
}

// Tagbodies always return NIL, hence the void
// return type
void eval_tagbody(List *env, List *tagbody_pairs, expression *tagbody_exp){
    tagbody_pairs = cons(make_pair(env, tagbody_exp->tagbody_value.tag_progn_map), tagbody_pairs);
    eval_exp(env, tagbody_pairs, tagbody_exp->tagbody_value.whole_progn);
    list_drop(tagbody_pairs);
}

boxed_value *eval_boxed(List *env, List* tagbody_env_pairs, boxed_value *bv){
    definition_expression def;
    call_expression call;
    boxed_value *value;
    expression *compiled_expression;
    List *exps;
    expression *exp;
    switch(bv->type){
        case boxed_int:
        case boxed_double:
        case boxed_string:
        case boxed_closure:
        case boxed_cons:
        case boxed_nil:
            value = bv;
            break;
        case boxed_expression:
            exp = bv->expression_value;
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
                case exp_string:
                    value = make_boxed_string(exp->string_value);
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
                        value = eval_exp(env, tagbody_env_pairs, def.exp);
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
                        value = eval_exp(env, tagbody_env_pairs, exps->car);
                        exps = exps->cdr;
                    }
                    break;
                case exp_list:
                    compiled_expression = compile_expression(exp);
                    value = eval_exp(env, tagbody_env_pairs, compiled_expression);
                    break;
                default:
                    die(make_msg("Unexpected expression type %d", exp->type));
            }
    }
    indent -=1;
    return value;
}

boxed_value *apply_primitive(List *env, List *tagbody_env_pairs, char *op_name, List *arg_exps){
    boxed_value *value;
    boxed_value *eval_sub_exp(expression *exp){
        return eval_exp(env, tagbody_env_pairs, exp);
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
        value = eval_sub_exp(nthelt(arg_exps, 1));
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
        value = eval_exp(tagbody_env, tagbody_env_pairs, (expression *)(hash_value(h, label)));
    } else if (op_name_equals("quote")) {
        die_unless(listlen(arg_exps) == 1, "quote requires one value");
        value = make_boxed_expression(arg_exps->car);
    } else {
        List *arg_values = list_map(arg_exps, (map_fn_ptr)eval_sub_exp);
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
        } else if (op_name_equals("list")) {
            List *l = reverse_list(arg_values);
            value = NIL;
            while(l != NULL){
                value = make_boxed_cons(l->car, value);
                l = l->cdr;
            }
            free_list(l, nop_free_fn);
        } else {
            die("Unexpected primitive");
        }
        free_list(arg_values, nop_free_fn);
    }
    return value;
}
boxed_value *apply(List *env, List *tagbody_env_pairs, expression *func_exp, List *arg_exps){
    boxed_value *value;

    if (func_exp->type == exp_identifier && is_primitive(func_exp->identifier_value))
        value = apply_primitive(env, tagbody_env_pairs, func_exp->identifier_value, arg_exps);
    else {
        boxed_value *func = eval_exp(env, tagbody_env_pairs, func_exp);
        boxed_value *eval_sub_exp(expression *exp){
            return eval_exp(env, tagbody_env_pairs, exp);
        }
        List *arg_values = list_map(arg_exps, (map_fn_ptr)eval_sub_exp);

        env = env_add_frame(env, copy_frame(func->closure_value.frame));
        Hash *frame = frame_create(func->closure_value.function.args, arg_values);
        env = env_add_frame(env, frame);
        value = eval_exp(env, tagbody_env_pairs, func->closure_value.function.body);
        env = env_drop_frame(env);
        env = env_drop_frame(env);
    }
    indent -=1;
    return value;
}

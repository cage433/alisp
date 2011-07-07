#include "compiler.h"
#include "list.h"
#include "expression.h"
#include "utils.h"

expression *process_expression(expression *exp);
char *identifier_name(expression *exp){
    die_unless(exp->type == exp_identifier, "Expected identifier expression");
    return strdup(exp->identifier_value);
}
expression *build_function_expression(List *list){
    expression *args_exp = list->car;
    die_unless(args_exp->type == exp_list, "Expected list of arguments");
    List *arguments = list_map(args_exp->list_value, (map_fn_ptr)identifier_name);
    expression *body = make_progn_expression(list_map(list->cdr, (map_fn_ptr)process_expression));
    return make_function_expression(arguments, body);
}

expression *build_lambda_expression(List *list){
    // drop lambda
    list = list->cdr;

    return build_function_expression(list);
}

expression *build_definition_expression(List *list){
    // drop 'def'
    list = list->cdr;

    expression *head = list->car;
    if (head->type == exp_list){
        List *definition_list = head->list_value;
        die_unless(listlen(definition_list) > 0, "Unexpected empty list in definition");
        List *identifiers = list_map(definition_list, (map_fn_ptr)identifier_name);
        char *name = identifiers->car;
        List *args = identifiers->cdr;
        expression *body = make_progn_expression(list_map(list->cdr, (map_fn_ptr)process_expression));
        return make_definition_expression(name, make_function_expression(args, body));

    } else if (head->type == exp_identifier){
        die_unless(listlen(list) == 2, "defined identifier requires exactly one expression after it");
        char *name = head->identifier_value;
        return make_definition_expression(name, process_expression(nthelt(list, 1)));
    } else {
        die(make_msg("Unexpected exp type %d", head->type));
    }
}

expression *build_progn_expression(List *list){
    // drop progn
    list = list->cdr;
    return make_progn_expression(list_map(list, (map_fn_ptr)process_expression));
}

expression *build_tagbody_expression(List *list){
    // drop tagbody
    list = list->cdr;

    // The tagbody is easier to build using a reversed list,
    // hence we don't map process_expression. 
    List *processed_exps = NULL;
    while (list != NULL){
        processed_exps = cons(process_expression(list->car), processed_exps);
        list = list->cdr;
    }

    List *progn_exps = NULL;
    Hash *hash = hash_create_with_string_keys();
    List *iter = processed_exps;

    while (iter != NULL){
        expression *exp = iter->car;
        if (exp->type == exp_identifier){
            hash_add(hash, exp->identifier_value, make_progn_expression(progn_exps));
        } else {
            progn_exps = cons(exp, progn_exps);
        }
        iter = iter->cdr;
    }


    expression *tagbody = make_tagbody_expression(
        make_progn_expression(progn_exps),
        hash
    );
    free_list(processed_exps, nop_free_fn);
    return tagbody;
}

expression *build_call_expression(List *list){
    List *processed_list = list_map(list, (map_fn_ptr)process_expression);
    expression *call_exp = make_call_expression(processed_list->car, processed_list->cdr);
    free(processed_list);
    return call_exp;
}

expression *process_expression(expression *exp){
    if (exp->type != exp_list){
        return exp;
    } else {
        List *list = exp->list_value;
        die_if(list == NULL, "Can't build expression from empty list");

        // Shuld either be an identifier expression in first place,
        // or else something that will evaluate to a function.
        
        expression *head = list->car;
        if (head->type == exp_identifier){
            char *identifier = head->identifier_value;
            if (strings_equal("def", identifier))
                return build_definition_expression(list);
            else if (strings_equal("lambda", identifier))
                return build_lambda_expression(list);
            else if (strings_equal("progn", identifier))
                return build_progn_expression(list);
            else if (strings_equal("tagbody", identifier))
                return build_tagbody_expression(list);
            else
                return build_call_expression(list);
        } else {
            return build_call_expression(list);
        }
    }
}
List *compile(List *expressions){
   return list_map(expressions, process_expression); 
}

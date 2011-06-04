#include "token.h"
#include "lexer.h"
#include "expression.h"
#include "list.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "string.h"
#include "utils.h"

expression *process_expression(expression *exp);

token *token_car(List *tokens){
    return (token *)(tokens->car);
}

void eat_left_paren(List **tokens){
    die_unless(token_car(*tokens)->type == tok_left_paren, "Expected '('");
    *tokens = (*tokens)->cdr;
}

void eat_right_paren(List **tokens){
    die_unless(token_car(*tokens)->type == tok_right_paren, "Expected ')'");
    *tokens = (*tokens)->cdr;
}

char *identifier_name(expression *exp){
    die_unless(exp->type == exp_identifier, "Expected identifier expression");
    return strdup(exp->identifier_value);
}
expression *consume_integer_exp(List **tokens){
    token *tok = token_car(*tokens);
    int num = tok->int_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_integer_expression(num);
    return exp;
}
    
expression *consume_double_exp(List **tokens){
    token *tok = token_car(*tokens);
    double num = tok->double_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_double_expression(num);
    return exp;
}

expression *consume_identifier_exp(List **tokens){
    token *tok = token_car(*tokens);
    char *identifier = tok->identifier_value;
    *tokens = (*tokens)->cdr;
    expression *exp = make_identifier_expression(identifier);
    return exp;
}

expression *build_function_expression(List *list){
    expression *args_exp = list->car;
    die_unless(args_exp->type == exp_list, "Expected list of arguments");
    List *arguments = list_map(args_exp->list_value, (map_fn_ptr)identifier_name);
    expression *body = make_progn_expression(list_map(list->cdr, (map_fn_ptr)process_expression));
    return make_function_expression(arguments, body);
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

expression *build_lambda_expression(List *list){
    // drop lambda
    list = list->cdr;

    return build_function_expression(list);
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



expression *consume_expression(List **tokens){
    token *tok = token_car(*tokens);
    if (tok->type == tok_integer)
        return consume_integer_exp(tokens);
    else if (tok->type == tok_double)
        return consume_double_exp(tokens);
    else if (tok->type == tok_identifier)
        return consume_identifier_exp(tokens);
    else if (tok->type == tok_left_paren){
        eat_left_paren(tokens);
        List *list = NULL;
        while (token_car(*tokens)->type != tok_right_paren){
            list = cons(consume_expression(tokens), list);
        }
        eat_right_paren(tokens);
        expression *exp = make_list_expression(reverse_list(list));
        free_list(list, nop_free_fn);
        return exp;
    }
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

List *parse_expressions(FILE *stream){
    
    List *tokens = getTokens(stream);
    List *tokens2 = tokens;
    List *expressions = NULL;
    while (tokens2 != NULL){
        expressions = cons(consume_expression(&tokens2), expressions);
    }
    List *unprocessed_expressions = reverse_list(expressions);
    List *processed_expressions = list_map(unprocessed_expressions, (map_fn_ptr)process_expression);
    free_list(expressions, nop_free_fn);
    free_list(tokens, (free_fn_ptr)free_token);
    free_list(unprocessed_expressions, nop_free_fn);
    return processed_expressions;
}

List *parse_expressions_from_string(char *text){
    FILE *s = fmemopen(text, strlen(text), "r");
    List *l = parse_expressions(s);
    return l;
}

expression *parse_expression_from_string(char *text){
    List *l = parse_expressions_from_string(text);
    die_unless(l != NULL && l->cdr == NULL, "List should have a single element");
    expression *exp = l->car;
    my_free(l);
    return exp;
}

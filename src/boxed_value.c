#include "boxed_value.h"
#include "utils.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "list.h"
#include "environment.h"
#include "hash.h"

int first_ptr = 0;
boxed_value *NIL = &(boxed_value){5, 0};
boxed_value *TRUE = &(boxed_value){0, 1};

int boxed_values_equal(const void *b1, const void *b2){
    boxed_value *box1 = (boxed_value *)b1;
    boxed_value *box2 = (boxed_value *)b2;
    if(box1->type != box2->type)
        return 0;
    if (box1->type == boxed_int)
        return box1->int_value == box2->int_value;
    else if (box1->type == boxed_double)
        return fabs(box1->double_value - box2->double_value) < 1e-9;
    else if (box1->type == boxed_string)
        return strcmp(box1->string_value, box2->string_value) == 0;
    else if (box1->type == boxed_cons)
        return boxed_values_equal(box1->cons_value.car, box2->cons_value.car) &&
                boxed_values_equal(box1->cons_value.cdr, box2->cons_value.cdr); 
    else if (box1->type == boxed_expression)
        return expressions_equal(box1->expression_value, box2->expression_value);
    else if (box1->type == boxed_nil)
        return 1;
    else {
        printf("File %s, line %d\n", __FILE__, __LINE__); 
        printf("Unexpected box type %d\n", box1->type);
        exit(-1);
    }
}
boxed_value *initialise_box_value(int type){
    boxed_value *box = my_malloc(sizeof(boxed_value));
    box->type = type;
    box->ref_count = 0;
    return box;
}


boxed_value *make_boxed_int(int num){
    boxed_value *box = initialise_box_value(boxed_int);
    box->int_value = num;
    return box;
}

boxed_value *make_boxed_double(double num){
    boxed_value *box = initialise_box_value(boxed_double);
    box->double_value = num;
    return box;
}

boxed_value *make_boxed_string(char *str){
    boxed_value *box = initialise_box_value(boxed_string);
    box->string_value = str;
    return box;
}

boxed_value *make_boxed_expression(expression *exp){
    boxed_value *box = initialise_box_value(boxed_expression);
    box->expression_value = exp;
    return box;
}

boxed_value *make_boxed_closure(List *env, function_expression func){
    boxed_value *box = initialise_box_value(boxed_closure);
    box->closure_value.frame = collapse_to_single_frame(env);
    box->closure_value.function = func;
    return box;
}

boxed_value *make_boxed_cons(boxed_value *car, boxed_value *cdr){
    boxed_value *box = initialise_box_value(boxed_cons);
    box->cons_value.car = car;
    box->cons_value.cdr = cdr;
    inc_ref_count(car);
    inc_ref_count(cdr);
    return box;
}
void free_boxed_value(boxed_value *b);

int constant_value(boxed_value *v){
    return v == NIL || v == TRUE;
}

void inc_ref_count(boxed_value *v){
    if (constant_value(v))
        return;
    v->ref_count += 1;
}
void dec_ref_count(boxed_value *v){
    if (constant_value(v))
        return;
    v->ref_count -= 1;
    if (v->ref_count == 0)
        free_boxed_value(v);
}

void free_boxed_value(boxed_value *b){
    switch (b->type){
        case boxed_int:
        case boxed_double:
            break;
        case boxed_string:
            my_free(b->string_value);
            break;
        case boxed_cons:
            dec_ref_count(b->cons_value.car);
            dec_ref_count(b->cons_value.cdr);
            break;
        case boxed_closure:
            //free_hash(b->closure_value.env);
            break;
        default:
            printf("Box type %d\n", b->type);
            die("Unexpected box type");
    }
    my_free(b);
}
void print_boxed_value(boxed_value *v, int indent){
    if (v == NIL){
        print_tabs(indent);
        printf("Boxed NIL\n");
    } else if (v == TRUE){
        print_tabs(indent);
        printf("Boxed TRUE\n");
    } else {
        switch (v->type){
            case boxed_int:
                print_tabs(indent);
                printf("%d\n", v->int_value);
                break;
            case boxed_double:
                print_tabs(indent);
                printf("%.6f\n", v->double_value);
                break;
            case boxed_string:
                print_tabs(indent);
                printf("%s\n", v->string_value);
                break;
            case boxed_closure:
                print_tabs(indent);
                printf("closure\n");
                break;
            case boxed_cons:
                print_tabs(indent);
                printf("cons\n");
                print_boxed_value(v->cons_value.car, indent + 1);
                print_boxed_value(v->cons_value.cdr, indent + 1);
                break;
            default:
                printf("File %s, line %d\n", __FILE__, __LINE__); 
                printf("box type %d\n", v->type);
                die("Unexpected box type");
        }
    }
}


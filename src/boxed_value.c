#include "boxed_value.h"
#include "utils.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

int boxed_values_equal(void *b1, void *b2){
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
    else {
        printf("File %s, line %d\n", __FILE__, __LINE__); 
        printf("Unexpected box type %d\n", box1->type);
        exit(-1);
    }
}

boxed_value *make_boxed_int(int num){
    boxed_value *box = (boxed_value *)malloc(sizeof(boxed_value));
    box->type = boxed_int;
    box->int_value = num;
    return box;
}

boxed_value *make_boxed_double(double num){
    boxed_value *box = (boxed_value *)malloc(sizeof(boxed_value));
    box->type = boxed_double;
    box->double_value = num;
    return box;
}

boxed_value *make_boxed_string(char *str){
    boxed_value *box = (boxed_value *)malloc(sizeof(boxed_value));
    box->type = boxed_string;
    box->string_value = str;
    return box;
}
void free_boxed_value(boxed_value *b){
    switch (b->type){
        case boxed_int:
        case boxed_double:
            break;
        case boxed_string:
            free(b->string_value);
            break;
 //       default:
//            die("Unexpected box type");
    }
}
boxed_value *make_boxed_definition(definition_expression def){
    boxed_value *box = malloc(sizeof(boxed_value));
    box->type = boxed_definition;
    box->definition_value = def;
    return box;
}

void print_boxed_value(boxed_value *v){
    switch (v->type){
        case boxed_int:
            printf("Boxed int %d\n", v->int_value);
            break;
        case boxed_double:
            printf("Boxed double %.6f\n", v->double_value);
            break;
        case boxed_string:
            printf("Boxed string %s\n", v->string_value);
            break;
        case boxed_definition:
            printf("Boxed definition %s\n", v->definition_value.name);
            break;
        default:
            printf("File %s, line %d\n", __FILE__, __LINE__); 
            die("Unexpected box type");
    }
}



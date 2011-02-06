#include <stdio.h>
#include <stdlib.h>
#include "stdarg.h"

#include "list.h"
#include "utils.h"
     
List *cons(void *car, List *list){
    List *consed_list = (List *)malloc(sizeof(List));
    consed_list->car = car;
    consed_list->cdr = list;
    return consed_list;
}
int listlen(List *list){
    List *l = list;
    int len = 0;
    while (l != NULL){
        l = l->cdr;
        ++len;
    }
    return len;
}

List *reverse_list(List *l){
    List *r;
    r = NULL;
    while (l != NULL){
        r = cons(l->car, r);
        l = l->cdr;
    }
    return r;
}

void free_list(List *l){
    while (l != NULL){
        List *l1 = l->cdr;
        free(l);
        l = l1;
    }
}

List *make_list(int size, ...){
    va_list(ap);
    va_start(ap, size);
    List *list = NULL;
    int i;
    for (i = 0; i < size; ++i){
        void *elt = va_arg(ap, void*);
        list = cons(elt, list);
    }
    List *result = reverse_list(list);
    free_list(list);
    return result;
}

int lists_equal(List *l1, List *l2, int(*isEq)(const void*, const void*)){
    while(l1 != NULL || l2 != NULL){
        if (l1 == NULL || l2 == NULL){
            return 0;
        }
        else if ((*isEq)(l1->car, l2->car)){
            l1 = l1->cdr;
            l2 = l2->cdr;
        } else {
            return 0;
        }
    }
    return 1;
}

void *nthelt(List *l, int n){
    die_unless(n >= 0, "nthelt called with negative argument");
    while (n >0 && l != NULL){
        --n;
        l = l->cdr;
    }
    die_if(l == NULL, "List too short");
    return l->car;
}

List *list_map(List *l, void *(*fn)(const void *)){
    List *rev = NULL;
    while (l != NULL){
        rev = cons(fn(l->car), rev);
        l = l->cdr;
    }
    List *result = reverse_list(rev);
    free_list(rev);
    return result;
}

void list_for_each(List *l, void(*fn)(const void *)){
    while (l != NULL){
        fn(l->car);
        l = l->cdr;
    }
}

void *list_fold(List *l, void *init, void *(*fn)(void *, const void *)){
    void *value = init;
    while (l != NULL){
        value = fn(value, l->car);
        l = l->cdr;
    }
    return value;
}

#include <stdio.h>
#include <stdlib.h>
#include "stdarg.h"

#include "list.h"
#include "utils.h"
     
List *cons(void *car, List *list){
    List *consed_list = my_malloc(sizeof(List));
    consed_list->car = car;
    consed_list->cdr = list;
    return consed_list;
}

int listlen(List *list){
    int len = 0;
    void inc(void *item){
        len += 1;
    }
    list_for_each(list, inc);
    return len;
}

List *reverse_list(List *l){
    void *rev_cons(void *list, void *item) {return cons(item, list);}
    return list_fold(l, NULL, rev_cons);
}

void free_list(List *l, void (*free_fn)(void *)){
    while (l != NULL){
        List *l1 = l->cdr;
        free_fn(l->car);
        my_free(l);
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
    free_list(list, nop_free_fn);
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
    free_list(rev, nop_free_fn);
    return result;
}

void list_for_each(List *l, void(*fn)(void *)){
    while (l != NULL){
        fn(l->car);
        l = l->cdr;
    }
}

void *list_fold(List *l, void *init, void *(*fn)(void *, void *)){
    void *value = init;
    while (l != NULL){
        value = fn(value, l->car);
        l = l->cdr;
    }
    return value;
}


int list_some(List *l, predicate_fn_ptr predicate){
    while (l != NULL){
        if (predicate(l->car))
            return 1;
        l = l->cdr;
    }
    return 0;
}

int list_all(List *l, predicate_fn_ptr predicate){
    while (l != NULL){
        if (!predicate(l->car))
            return 0;
        l = l->cdr;
    }
    return 1;
}

List *list_find(List *l, predicate_fn_ptr predicate){
    while (l != NULL){
        if (predicate(l->car))
            return l;
        l = l->cdr;
    }
    return NULL;
}

List *list_filter(List *l, predicate_fn_ptr predicate){
    List *rev_result = NULL;
    while (l != NULL){
        if (predicate(l->car))
            rev_result = cons(l->car, rev_result);
    }
    List *result = reverse_list(rev_result);
    free_list(rev_result, nop_free_fn);
    return result;
}

List *list_filter_not(List *l, predicate_fn_ptr predicate){
    int not_predicate(void *thing){
        if (predicate(thing) == 0)
            return 1;
        else
            return 0;
    }
    return list_filter(l, not_predicate);
}

void *list_last_element(List *l){
    die_if(l == NULL, "List is empty");
    while (l->cdr != NULL)
        l = l->cdr;
    return l->car;
}

List *list_drop_with_free_fn(List *l, free_fn_ptr free_fn){
    die_if(l == NULL, "Can't drop head of empty list");
    List *l2 = l->cdr;
    free_fn(l->car);
    free(l->car);
    free(l);
    return l2;
}

List *list_drop(List *l){
    list_drop_with_free_fn(l, nop_free_fn);
}

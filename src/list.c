#include <stdio.h>
#include <stdlib.h>

#include "list.h"
     
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
                printf("Freeing\n");
                List *l1 = l->cdr;
                free(l);
                l = l1;
        }
}

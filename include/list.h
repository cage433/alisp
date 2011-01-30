#ifndef ALISP_LIST
#define ALISP_LIST

typedef struct List{
        void *car;
        struct List *cdr;
} List;


List *cons(void *car, List *list);
int listlen(List *list);
List *reverse_list(List *l);
void free_list(List *l);
List *make_list(int size, ...);

#endif

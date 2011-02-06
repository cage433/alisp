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
int lists_equal(List *l1, List *l2, int (*isEq)(const void*, const void*));
void *nthelt(List *l, int n);
List *list_map(List *l, void *(*fn)(const void *));
void list_for_each(List *l, void(*fn)(void *));
/**
 * Note that first parameter is not const - this will be the accumulator
 * and we may want to free that at each step and return a new one
 */
void *list_fold(List *l, void *init, void *(*fn)(void *, const void *));
#endif

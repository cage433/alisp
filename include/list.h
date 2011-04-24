#ifndef ALISP_LIST
#define ALISP_LIST

typedef struct List{
    void *car;
    struct List *cdr;
} List;


List *cons(void *car, List *list);
int listlen(List *list);
List *reverse_list(List *l);
typedef void (*free_fn_ptr)(void *);
void free_list(List *l, free_fn_ptr fn);
List *make_list(int size, ...);
int lists_equal(List *l1, List *l2, int (*isEq)(const void*, const void*));
void *nthelt(List *l, int n);

typedef void *(*map_fn_ptr)(const void *);
List *list_map(List *l, map_fn_ptr fn);

typedef void(*for_each_fn_ptr)(void *);
void list_for_each(List *l, for_each_fn_ptr fn);

typedef void *(*fold_fn_ptr)(void *, void *);
void *list_fold(List *l, void *init, fold_fn_ptr fn);

typedef int(*predicate_fn_ptr)(void *);
int list_some(List *l, predicate_fn_ptr predicate);
int list_all(List *l, predicate_fn_ptr predicate);
List *list_find(List *l, predicate_fn_ptr predicate);
List *list_filter(List *l, predicate_fn_ptr predicate);
List *list_filter_not(List *l, predicate_fn_ptr predicate);
void *list_last_element(List *l);
List *list_drop_with_free_fn(List *l, free_fn_ptr free_fn);
List *list_drop(List *l);
#endif

#include "pair.h"
#include "stdlib.h"

pair *make_pair(void *lhs, void *rhs){
    pair *p = malloc(sizeof(pair));
    p->lhs = lhs;
    p->rhs = rhs;
    return p;
}

#ifndef ALISP_PAIR
#define ALISP_PAIR

typedef struct pair{
    void *lhs;
    void *rhs;
} pair;


pair *make_pair(void *lhs, void *rhs);

#endif

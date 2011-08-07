#ifndef ALISP_UTILS
#define ALISP_UTILS

#include "stdio.h"
#include "stdlib.h"
#include "setjmp.h"

char *make_msg(char *fmt, ...);
void die(char *msg);

void die_if(int predicate, char* message);

void die_unless(int predicate, char* message);
int strings_equal(const void *v1, const void * v2);
void nop_free_fn(void *);
void *my_malloc(size_t size);
void *my_free(void *ptr);
void print_tabs(int n);
extern jmp_buf buf;
#endif

#include "utils.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

char *make_msg(char *fmt, ...){
    char *buf = malloc(1000 * sizeof(char));
    va_list(ap);
    va_start(ap, fmt);
    vsnprintf(buf, 1000, fmt, ap);
    va_end(ap);

    char *smallbuf = strdup(buf);
    free(buf);
    return smallbuf;
}

void die(char *msg){
    printf("Exiting: %s\n", msg);
    longjmp(jmp_buffer, 1);
}


void die_if(int predicate, char* msg){
    if(predicate)
        die(msg);
}

void die_unless(int predicate, char* message){
    die_if(!predicate, message);
}

int strings_equal(const void *v1, const void * v2){
    return strcmp((char *)v1, (char *)v2) == 0;
}

void nop_free_fn(void *item){
}
void *my_malloc(size_t size){
    void *ptr = malloc(size);
    return ptr;
}
void *my_free(void *ptr){
    free(ptr);
}
void print_tabs(int n){
    int i;
    for (i = 0; i < n; ++i)
        printf("\t");
}

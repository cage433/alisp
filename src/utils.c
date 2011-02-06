#include "utils.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void die(char *msg){
    printf("Exiting: %s\n", msg);
    exit(-1);
}


void die_if(int predicate, char* message){
    if(predicate)
        die(message);
}

void die_unless(int predicate, char* message){
    die_if(!predicate, message);
}

int strings_equal(const void *v1, const void * v2){
    return strcmp((char *)v1, (char *)v2) == 0;
}

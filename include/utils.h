#ifndef ALISP_UTILS
#define ALISP_UTILS

#include "stdio.h"
#include "stdlib.h"

void die(char *msg);

void die_if(int predicate, char* message);

void die_unless(int predicate, char* message);
#endif

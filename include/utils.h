#ifndef ALISP_UTILS
#define ALISP_UTILS

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
#endif

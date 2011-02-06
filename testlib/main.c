//#include "test.h"
#include "stdio.h"
#include "dlfcn.h"


//void call_lib(){
//    double x;
//    x = 3.0;
//    printf("Twice %'.2f = %'.2f\n", x, twice(x));
//}

void main(){
    void *handle;
    handle = dlopen("test.so", RTLD_LAZY);
    if (handle == NULL){
        printf("Failed to open for reson %s\n", dlerror());
    } else {
        printf("Library opened successfully!!\n");
    }
    dlerror();
    void *fun_ptr = dlsym(handle, "twice");
    char *error = dlerror();
    if (error != NULL){
        printf("Failed to find function, %s\n", error);
    } else {
        printf("Have loaded function!!!\n");
    }
    double(*fun)(double) = fun_ptr;
    double six = fun(3.0);
    printf("Six = %'.2f\n", six);

    dlerror();
    void *fred_ptr = dlsym(handle, "FRED");

    error = dlerror();
    if (error != NULL){
        printf("Failed to find FRED, %s\n", error);
    } else {
        printf("Have loaded FRED!!!\n");
    }
    int *fred = (int *)fred_ptr;
    printf("FRED = %d\n", *fred);
}


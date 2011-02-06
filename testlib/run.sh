#!/bin/sh
gcc -shared test.c -o test.so
gcc main.c -ldl
export LD_LIBRARY_PATH=.
./a.out

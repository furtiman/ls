#! /bin/sh

CFLAGS="-Wall -g -c -O2 "
LFLAGS="-lm"

echo "compiling..."
gcc $CFLAGS auriga_test_3.c || exit

echo "linking..."
gcc -o auriga_test_3 auriga_test_3.o $LFLAGS || exit

#!/bin/sh
rm tester
gcc -c tester.c
gcc -o tester tester.o -llrsp
rm tester.o

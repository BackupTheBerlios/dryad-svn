rm module_test
cp ../../trunk/src/dstring.h .
cp ../../trunk/src/dstring.cpp .
cp ../../trunk/src/drarray.h .
cp ../../trunk/src/drarray.cpp .
cp ../../trunk/src/syslog.h .
cp ../../trunk/src/functions.h .
cp ../../trunk/src/functions.cpp .
gcc -c module_test.cpp
gcc -c dstring.cpp
gcc -c drarray.cpp
gcc -c functions.cpp
g++ -o module_test module_test.o dstring.o drarray.o functions.o -ldl -rdynamic
rm *.o
rm dstring.*
rm drarray.*
rm syslog.h
rm functions.*

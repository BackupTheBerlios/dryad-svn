rm module_test
cp ../../trunk/src/dstring.h .
cp ../../trunk/src/dstring.cpp .
cp ../../trunk/src/drarray.h .
cp ../../trunk/src/drarray.cpp .
cp ../../trunk/src/syslog.h .
cp ../../trunk/src/functions.h .
cp ../../trunk/src/functions.cpp .
cp ../../trunk/src/conf.h .
cp ../../trunk/src/conf.cpp .
cp ../../trunk/src/dfilestream.h .
cp ../../trunk/src/dfilestream.cpp .
cp ../../trunk/src/database.h .
cp ../../trunk/src/database.cpp .
gcc -c module_test.cpp
gcc -c dstring.cpp
gcc -c dfilestream.cpp
gcc -c database.cpp
gcc -c drarray.cpp
gcc -c functions.cpp
gcc -c conf.cpp
g++ -o module_test module_test.o dstring.o dfilestream.o database.o conf.o drarray.o functions.o -ldl -rdynamic -lpcre
rm *.o
rm dstring.*
rm drarray.*
rm syslog.h
rm functions.*
rm conf.*
rm dfilestream.*
rm database.*

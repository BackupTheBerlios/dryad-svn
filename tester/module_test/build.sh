rm module_test
cp ../../dryad/trunk/src/dstring.h .
cp ../../dryad/trunk/src/dstring.cpp .
cp ../../dryad/trunk/src/drarray.h .
cp ../../dryad/trunk/src/drarray.cpp .
cp ../../dryad/trunk/src/syslog.h .
cp ../../dryad/trunk/src/functions.h .
cp ../../dryad/trunk/src/functions.cpp .
cp ../../dryad/trunk/src/conf.h .
cp ../../dryad/trunk/src/conf.cpp .
cp ../../dryad/trunk/src/dfilestream.h .
cp ../../dryad/trunk/src/dfilestream.cpp .
cp ../../dryad/trunk/src/database.h .
cp ../../dryad/trunk/src/database.cpp .
cp ../../dryad/trunk/src/dryerr.h .
cp ../../dryad/trunk/src/dryerr.cpp .
gcc -c dryerr.cpp
gcc -c module_test.cpp
gcc -c dstring.cpp
gcc -c dfilestream.cpp
gcc -c database.cpp
gcc -c drarray.cpp
gcc -c functions.cpp
gcc -c conf.cpp
g++ -o module_test dryerr.o module_test.o dstring.o dfilestream.o database.o conf.o drarray.o functions.o -ldl -rdynamic -lpcre
rm *.o
rm dryerr.*
rm dstring.*
rm drarray.*
rm syslog.h
rm functions.*
rm conf.*
rm dfilestream.*
rm database.*

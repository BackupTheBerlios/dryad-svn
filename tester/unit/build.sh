rm dcache_unit
cp ../../dryad/trunk/src/dstring.h .
cp ../../dryad/trunk/src/dstring.cpp .
cp ../../dryad/trunk/src/drarray.h .
cp ../../dryad/trunk/src/drarray.cpp .
cp ../../dryad/trunk/src/syslog.h .
cp ../../dryad/trunk/src/functions.h .
cp ../../dryad/trunk/src/functions.cpp .
cp ../../dryad/trunk/src/cache.h .
cp ../../dryad/trunk/src/cache.cpp .
cp ../../dryad/trunk/src/dfilestream.h .
cp ../../dryad/trunk/src/dfilestream.cpp .
cp ../../dryad/trunk/src/database.h .
cp ../../dryad/trunk/src/database.cpp .
gcc -c dcache_unit.cpp
gcc -c dstring.cpp
gcc -c dfilestream.cpp
gcc -c database.cpp
gcc -c drarray.cpp
gcc -c functions.cpp
gcc -c cache.cpp
g++ -o dcache_unit dcache_unit.o dstring.o dfilestream.o database.o cache.o drarray.o functions.o -ldl -rdynamic -lpcre
rm *.o
rm dstring.*
rm drarray.*
rm syslog.h
rm functions.*
rm cache.*
rm dfilestream.*
rm database.*

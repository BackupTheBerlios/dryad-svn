#!/bin/sh

function die()
{
	killall dryad > /dev/null 2>&1
	echo "error!"
	cd $CWD
	echo "Don't forget to make distclean!"
	exit 1
}

function build_config()
{
	cp "$CWD/reg_test.cnf.dist" "$CWD/reg_test.cnf"
	echo "dbfile $CWD/reg_test.db" >> "$CWD/reg_test.cnf"
}

function add_config_line()
{
	echo "$1" > "$CWD/reg_test.cnf"
}

function sleep_5()
{
	echo -n "sleeping for 5 seconds. "
	sleep 1s
	echo -n "4 "
	sleep 1s
	echo -n "3 "
	sleep 1s
	echo -n "2 "
	sleep 1s
	echo -n "1 "
	sleep 1s
	echo -n "0"
}

function sekret_test()
{
	echo -n "testing with -s $1 as parameters: "
	./dryad -c $CWD/reg_test.cnf -s $1 > reg_test.sek 2>&1 || die
	if [ "`cat reg_test.sek`" != "$2" ]; then
		die
	fi
	echo "passed."
	rm reg_test.sek
}

function full_run_test()
{
	echo -n "testing full run with $1 as parameters: "
	./dryad -c $CWD/reg_test.cnf $1 > reg_test.full 2>&1 &
	PID=`ps ax | grep dryad | grep -v grep | awk '{print $1}'`
	if [ -z "$PID" ]; then
		echo "failed to find pid!"
		die
	fi
	echo -n "process has pid of $PID... "
	sleep_5
	echo -n " Killing $PID: "
	kill $PID || die
	echo "done."
	sleep 2s
}

function valid_test()
{
	echo -n "testing handling with $1 switches: "
	$CWD/src/dryad -c $CWD/reg_test.cnf $1 > reg_test.out 2> /dev/null &
	PID=`ps ax | grep dryad | grep -v grep | awk '{print $1}'`
	if [ -z "$PID" ]; then
		echo "failed to find pid!"
		die
	fi
	sleep 1s
	./tester 127.0.0.1 $2 "<0>May 11 20:29:47 localhost [su(pam_unix)] session opened for user root by (uid=0)" > /dev/null 2>&1 || die
	if [ "`cat reg_test.out`" != "NULL--May 11 20:29:47--0--localhost--[su(pam_unix)] session opened for user root by (uid=0)--0" ]; then
		kill $PID
		die
	fi
	kill $PID
	rm reg_test.out
}

CWD=`pwd`

echo -n "Running autotools: "
./autogen.sh > /dev/null 2>&1 || die
echo "done."
echo -n "Running configure: "
LDFLAGS="-rdynamic" CXXFLAGS="-O0 -g3" ./configure --enable-debug=full > /dev/null 2>&1 || die
echo "done."
echo -n "Building source: "
make -j4  > /dev/null 2>&1 || die
echo "done."

cd src
export LD_LIBRARY_PATH="$CWD/src/plugins/.libs/"

build_config

# first we test that udp mode starts
sekret_test "-u 1234" "tcp: -1--udp: 1234"

# then we test tcp (lrsp)
sekret_test "-t 1234" "tcp: 1234--udp: -1"

# test udp and tcp at the same time
sekret_test "-t 1234 -u 1234" "tcp: 1234--udp: 1234"


# now we do more intensive tests, actually letting dryad start
full_run_test "-u 1234"
full_run_test "-t 1234"
full_run_test "-u 1234 -t 1234"

# now we need to build our tools to send test messages
cd $CWD/../../tester/udp/
echo -n "Building the udp tester: "
./build.sh || die
echo "done."
valid_test "-u 1234" "1234"
cd $CWD/src

cd $CWD/../../tester/lrsp/
echo -n "Building the lrsp tester: "
./build.sh || die
echo "done."
valid_test "-t 1234" "1234"
cd $CWD/src


unset LD_LIBRARY_PATH
cd ..
rm reg_test.cnf
make distclean > /dev/null
echo "Hoorjay! All tests passed!"



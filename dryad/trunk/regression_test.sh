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
	kill $PID >/dev/null 2>&1 || die
	rm reg_test.full
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
		kill >/dev/null 2>&1 $PID
		die
	fi
	kill >/dev/null 2>&1 $PID
	rm reg_test.out
	echo " done."
	sleep 2s
}

function invalid_test()
{
	echo -n "testing handling of invalid syslog message with $1 switches: "
	$CWD/src/dryad -c $CWD/reg_test.cnf $1 > reg_test.out 2> /dev/null &
  PID=`ps ax | grep dryad | grep -v grep | awk '{print $1}'`
	if [ -z "$PID" ]; then
		echo "failed to find pid!"
		die
	fi
	sleep 1s
	./tester 127.0.0.1 $2 "Blhabdfshdfgjrlhablah invalid message pr0n monkies. su root" > /dev/null 2>&1 || die
	if [ "`cat reg_test.out`" ]; then
		kill >/dev/null 2>&1 $PID
		die
	fi
	kill >/dev/null 2>&1 $PID
	rm reg_test.out
	echo " done."
	sleep 2s
}

function length_test()
{
	echo -n "testing the handling of overly long messages with $1 switches: "
	$CWD/src/dryad -c $CWD/reg_test.cnf $1 > reg_test.out 2>/dev/null &
	PID=`ps ax|grep dryad|grep -v grep|awk '{print $1}'`
	if [ -z "$PID" ]; then
		echo "failed to find pid!"
		die
	fi
	sleep 1s
	./tester 127.0.0.1 $2 "<0>May 11 20:29:47 localhost [su(pam_super_unix_wombats_of_death_doom_and_despair)] session opened for the_user_with_a_really_far_far_far_far_far_far_far_far_far_to_long_name_or_should_that_be_too_rather_than_to_I_dont_know_maybe_I_should_have_spent_less_time_sleeping_in_English_in_High_School_and_University_but_that_would_be_far_to_logical_an_explanation_now_wouldnt_it_no_I_must_blame_this_on_the_badgers_of_DOOM_I_wonder_how_close_to_done_I_am_I_should_test_with_wc_alas_I_am_only_at_473_oh_NOEZ_WHATEVAR_SHALL_I_DO_I_Know_Ill_repeat_some_sections_here_goes_the_user_with_a_really_far_far_far_far_far_far_far_far_far_to_long_name_or_should_that_be_too_rather_than_to_I_dont_know_maybe_I_should_have_spent_less_time_sleeping_in_English_in_High_School_and_University_but_that_would_be_far_to_logical_an_explanation_now_wouldnt_it_no_I_must_blame_this_on_the_badgers_of_DOOM_I_wonder_how_close_to_done_I_am_I_should_test_with_wc_alas_I_am_only_at_473_Is_that_good_I_wonder_closer_at_973_but_not_quite_enough_well_damn_I_am_totally_out_of_ideas_at_this_point_other_than_to_say_that_anyone_who_has_a_daemon_that_creates_this_much_output_is_totally_insane_and_I_will_cry_if_I_evar_meet_him_or_her_this_damn_well_better_be_enough by (uid=1234)" > /dev/null 2>&1
	# we should expect the first 1024 bytes, and that's it.
	if [ "`cat reg_test.out`" != "NULL--May 11 20:29:47--0--localhost--[su(pam_super_unix_wombats_of_death_doom_and_despair)] session opened for the_user_with_a_really_far_far_far_far_far_far_far_far_far_to_long_name_or_should_that_be_too_rather_than_to_I_dont_know_maybe_I_should_have_spent_less_time_sleeping_in_English_in_High_School_and_University_but_that_would_be_far_to_logical_an_explanation_now_wouldnt_it_no_I_must_blame_this_on_the_badgers_of_DOOM_I_wonder_how_close_to_done_I_am_I_should_test_with_wc_alas_I_am_only_at_473_oh_NOEZ_WHATEVAR_SHALL_I_DO_I_Know_Ill_repeat_some_sections_here_goes_the_user_with_a_really_far_far_far_far_far_far_far_far_far_to_long_name_or_should_that_be_too_rather_than_to_I_dont_know_maybe_I_should_have_spent_less_time_sleeping_in_English_in_High_School_and_University_but_that_would_be_far_to_logical_an_explanation_now_wouldnt_it_no_I_must_blame_this_on_the_badgers_of_DOOM_I_wonder_how_close_to_done_I_am_I_should_test_with_wc_alas_I_am_only_at_473_Is_that_good_I_wonder_closer_at_973_but_not_quite_enough_well_damn_I_am--0" ]; then
		kill $PID >/dev/null 2>&1 || die
		die
	fi
	kill $PID >/dev/null 2>&1 || die
	rm reg_test.out
	echo " done."
	sleep 2s
}

function multi_message_test()
{
	echo -n "testing sending multiple messages with $1 switches: "
	$CWD/src/dryad -c $CWD/reg_test.cnf $1 > reg_test.out 2>/dev/null &
	PID=`ps ax | grep dryad | grep -v grep | awk '{print $1}'`
	if [ -z "$PID" ]; then
		echo "failed to find pid!"
		die
	fi
	sleep 1s
	./tester 127.0.0.1 $2 "<0>May 11 20:29:47 localhost [su(pam_unix)] session opened for user root by (uid=0)" > /dev/null 2>&1 || die
	./tester 127.0.0.1 $2 "<0>May 11 20:29:47 localhost [su(pam_unix)] session closed for user root by (uid=0)" > /dev/null 2>&1 || die
	if [ "`cat reg_test.out | head -n 1`" != "NULL--May 11 20:29:47--0--localhost--[su(pam_unix)] session opened for user root by (uid=0)--0" ]; then
		kill $PID > /dev/null 2>&1 || die
		die
	fi
	if [ "`cat reg_test.out | tail -n 1`" != "NULL--May 11 20:29:47--0--localhost--[su(pam_unix)] session closed for user root by (uid=0)--0" ]; then
		kill $PID > /dev/null 2>&1 || die
		die
	fi
	kill $PID > /dev/null 2>&1 || die
	rm reg_test.out
	echo "done."
	sleep 2s
}

function persistant_test()
{
	echo -n "testing lrsp persistant mode connections: "
	$CWD/src/dryad -c $CWD/reg_test.cnf $1 > reg_test.out 2>/dev/null &
	PID=`ps ax | grep dryad | grep -v grep | awk '{print $1}'`
	if [ -z "$PID" ]; then
		echo "failed to find pid!"
		die
	fi
	sleep 1s
  ./tester 127.0.0.1 $2 "<0>May 11 20:29:47 localhost [su(pam_unix)] session opened for user root by (uid=0)" 2 > /dev/null 2>&1 || die
	if [ "`cat reg_test.out | head -n 1`" != "NULL--May 11 20:29:47--0--localhost--[su(pam_unix)] session opened for user root by (uid=0)--0" ]; then
		kill $PID > /dev/null 2>&1 || die
		die
	fi
	if [ "`cat reg_test.out | tail -n 1`" != "NULL--May 11 20:29:47--0--localhost--[su(pam_unix)] session opened for user root by (uid=0)--0" ]; then
		kill $PID > /dev/null 2>&1 || die
		die
	fi
	kill $PID > /dev/null 2>&1 || die
	rm reg_test.out
	echo "done."
	sleep 2s
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

# now we send invalid messages, they should be discarded and no output created, even though they match the patterns
cd $CWD/../../tester/udp/
invalid_test "-u 1234" "1234"
cd $CWD/src
# we don't need to test tcp, 'cause the test is for the message parse code

# now we test input that is to long (>1024 bytes)
cd $CWD/../../tester/udp/
length_test "-u 1234" "1234"
cd $CWD/src

echo -n "I'm going to sleep for 60 seconds as this next test follows to closely on the heels of the previous tcp test, and can't bind the port as a result: "
sleep 60s
echo "done."
cd $CWD/../../tester/lrsp/
length_test "-t 1234" "1234"
cd $CWD/src


# now we test sending multiple messages
cd $CWD/../../tester/udp/
multi_message_test "-u 1234" "1234"
cd $CWD/src

echo -n "Again, I'm going to sleep for 60 seconds: "
sleep 60s
echo "done."
cd $CWD/../../tester/lrsp/
multi_message_test "-t 1234" "1234"
cd $CWD/src

# and, finally, lrsp persistance
echo -n "Again, sleeping for 60 seconds: "
sleep 60s
echo "done."
cd $CWD/../../tester/lrsp/
persistant_test "-t 1234" "1234"
cd $CWD/src

unset LD_LIBRARY_PATH
cd ..
rm reg_test.cnf
make distclean > /dev/null
echo "Hoorjay! All tests passed!"



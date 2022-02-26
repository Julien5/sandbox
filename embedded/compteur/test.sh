#!/usr/bin/env bash

set -e
#set -x

export TEST_CURL_SH=./test-curl.sh

function reset() {
	unset START_TIME
	unset TICK_PERIOD
	unset PERIODS_FILE
	unset SIMULATION_DURATION
}

out=out # $(mktemp)
function run() {
	echo running...
	rm -f $out
	/tmp/build_pc/compteur/compteur &> $out || true
	NTRANSMIT=$( grep $TEST_CURL_SH $out | wc -l)
	NTRANSMIT_DATA=$(grep $TEST_CURL_SH $out | grep tickcounter | wc -l)
	NTRANSMIT_EPOCH=$(grep $TEST_CURL_SH $out | grep epoch | wc -l)
}

function print() {
	printf "there were %d transmissions\n" $NTRANSMIT
	printf "there were %d tickscounter transmissions\n" $NTRANSMIT_DATA
	printf "there were %d epoch transmissions\n" $NTRANSMIT_EPOCH
}

function check() {
	name=$1
	expected=$2
	actual=$3
	if [[ "$actual" = "$expected" ]]; then
		printf "[OK] %s %s %s\n" $name
		return 0;
	fi
	printf "[NO] %s should be %s\n" $name $expected
	exit 1;
}

function test_default() {
	echo default
	reset
	run
	print
	check NTRANSMIT_DATA 17 $NTRANSMIT_DATA
	check NTRANSMIT_EPOCH 18 $NTRANSMIT_EPOCH
}

function test_ticks_soon() {
	echo ticks soon
	reset
	export START_TIME=$((10*3600+123))
	export TICK_PERIOD=7 # <10 
	run
	print
	check NTRANSMIT_DATA 0 $NTRANSMIT_DATA
	check NTRANSMIT_EPOCH 1 $NTRANSMIT_EPOCH
}

function test_large_delta() {
	echo large delta
	reset
	export START_TIME=$((10*3600))
	export PERIODS_FILE="periods-large-delta.txt"
	export SIMULATION_DURATION=$((3600*1000*15/10));
	run
	print
	check NTRANSMIT_DATA 4 $NTRANSMIT_DATA
	check NTRANSMIT_EPOCH 5 $NTRANSMIT_EPOCH
}

function test_full() {
	echo full
	reset
	export SIMULATION_DURATION=$((3600*1000*21/10));
	export START_TIME=$((10*3600))
	export TICK_PERIOD=380
	run
	print
	check NTRANSMIT_DATA 2 $NTRANSMIT_DATA
	check NTRANSMIT_EPOCH 3 $NTRANSMIT_EPOCH
	export TICK_PERIOD=340
	run
	print
	check NTRANSMIT_DATA 4 $NTRANSMIT_DATA
	check NTRANSMIT_EPOCH 5 $NTRANSMIT_EPOCH
}

function test_night() {
	echo night
	reset
	export TICK_PERIOD=100
	export SIMULATION_DURATION=$((3600*1000*59/10));
	run
	print
	check NTRANSMIT_DATA 0 $NTRANSMIT_DATA
	check NTRANSMIT_EPOCH 1 $NTRANSMIT_EPOCH
	export SIMULATION_DURATION=$((3600*1000*61/10));
	run
	print
	check NTRANSMIT_DATA 1 $NTRANSMIT_DATA
	check NTRANSMIT_EPOCH 2 $NTRANSMIT_EPOCH
}

test_default
test_ticks_soon
test_large_delta
test_full
test_night
echo good

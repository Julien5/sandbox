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
	rm -f $out
	/tmp/build_pc/compteur/compteur | tee $out || true
	NTRANSMIT=$( grep $TEST_CURL_SH $out | wc -l)
	NTRANSMIT_DATA=$(grep $TEST_CURL_SH $out | grep tickcounter | wc -l)
	NTRANSMIT_EPOCH=$(grep $TEST_CURL_SH $out | grep epoch | wc -l)
}

function print() {
	printf "there were %d transmissions\n" $NTRANSMIT
	printf "there were %d tickscounter transmissions\n" $NTRANSMIT_DATA
	printf "there were %d epoch transmissions\n" $NTRANSMIT_EPOCH
}

function test_ticks_soon() {
	reset
	export START_TIME=$((10*3600+123))
	export TICK_PERIOD=7
	export SIMULATION_DURATION=$((3600*1000*31/10));
	run
	print
}

test_ticks_soon

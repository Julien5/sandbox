#!/usr/bin/env bash

set -e
#set -x

export TEST_CURL_SH=./test-curl.sh
export EPOCH=$((1643673600+20*3600))
export TICK_PERIOD=600;
export PERIODS_FILE=periods.txt
export SIMULATION_END_TIME=$((3600*1000));
out=out # $(mktemp)
/tmp/build_pc/compteur/compteur | tee $out || true
NTRANSMIT=$( grep $TEST_CURL_SH $out | wc -l)
#rm -f $out
printf "there were %d transmissions\n" $NTRANSMIT

NTRANSMIT=$(grep $TEST_CURL_SH $out | grep tickcounter | wc -l)
#rm -f $out
printf "there were %d tickscounter transmissions\n" $NTRANSMIT

NTRANSMIT=$(grep $TEST_CURL_SH $out | grep epoch | wc -l)
#rm -f $out
printf "there were %d epoch transmissions\n" $NTRANSMIT

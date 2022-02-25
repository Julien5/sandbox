#!/usr/bin/env bash

set -e
#set -x

export TEST_CURL_SH=./test-curl.sh
export EPOCH=$((1643673600+0*3600))
export TICK_PERIOD=600;
out=out # $(mktemp)
/tmp/build_pc/compteur/compteur | tee $out || true
NTRANSMIT=$( grep $TEST_CURL_SH $out | wc -l)
#rm -f $out
printf "there were %d transmissions in 24 hours\n" $NTRANSMIT

NTRANSMIT=$(grep $TEST_CURL_SH $out | grep tickcounter | wc -l)
#rm -f $out
printf "there were %d tickscounter transmissions in 24 hours\n" $NTRANSMIT

NTRANSMIT=$(grep $TEST_CURL_SH $out | grep epoch | wc -l)
#rm -f $out
printf "there were %d epoch transmissions in 24 hours\n" $NTRANSMIT

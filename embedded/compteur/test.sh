#!/usr/bin/env bash

set -e
#set -x

export TEST_CURL_SH=./test-curl.sh
export EPOCH=123456
export TICK_PERIOD=600;
out=out # $(mktemp)
/tmp/build_pc/compteur/compteur &> $out || true
NTRANSMIT=$(grep "wifi_curl_pc.cpp:45" $out | wc -l)
#rm -f $out
printf "there were %d transmissions in 24 hours\n" $NTRANSMIT

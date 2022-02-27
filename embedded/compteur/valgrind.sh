#!/usr/bin/env bash

set -e
set -x

export TEST_CURL_SH=./test-curl.sh
export START_TIME=$((10*3600))
export SIMULATION_DURATION=$((3600*1000*15/10));
PROG=/tmp/build_pc/compteur/compteur
valgrind --tool=massif \
		 --detailed-freq=1 \
		 --time-unit=ms \
		 --stacks=yes \
		 --threshold=0.0 \
		 --massif-out-file=massif.out \
		 $PROG

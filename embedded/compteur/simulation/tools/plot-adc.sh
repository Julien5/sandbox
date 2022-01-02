#!/usr/bin/env bash

set -e

DIR=/tmp/replay
mkdir -p /tmp/replay

if [[ ! -f $DIR/out ]]; then
	echo regenerate $DIR/out
	/tmp/build_pc/compteur/compteur > $DIR/out
fi

if [[ ! -f $DIR/values.mean ]]; then
	echo regenerate $DIR/values.mean
	cat $DIR/out | grep detection.cpp | grep value | grep time | while read a; do 
		t=$(echo $a | cut -f5 -d: | cut -f1 -d" ");
		v=$(echo $a | cut -f6 -d:);
		ts=$(echo "scale=3;$t/1000.0" | bc)
		echo $ts $v; done > $DIR/values.mean
fi

gnuplot ./simulation/tools/replay.gnuplot
killall feh || true
feh replay.png &


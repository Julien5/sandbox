#!/usr/bin/env bash

set -e
#set -x

DIR=/tmp/replay
mkdir -p /tmp/replay

if [[ ! -f $DIR/out || "$1" = "build" ]]; then
	find $DIR/ -type f -delete -print
	echo regenerate $DIR/out
	/tmp/build_pc/compteur/compteur > $DIR/out
fi

if [[ ! -f $DIR/values.mean ]]; then
	echo regenerate $DIR/values.mean
	cat $DIR/out | grep detection.cpp | grep value | grep time | while read a; do 
		t=$(echo $a | cut -f5 -d: | cut -f1 -d" ");
		ts=$(echo "scale=3; $t/1000.0" | bc)
		v=$(echo $a | cut -f6 -d:);
		echo $ts $v;
	done > $DIR/values.mean
fi

if [[ ! -f $DIR/ticks ]]; then
	echo regenerate $DIR/ticks
	cat $DIR/out | grep TICK | while read a; do
		t=$(echo $a | cut -f5 -d: | cut -f1 -d" ");
		ts=$(echo "scale=3; $t/1000.0" | bc)
		TL=$(echo $a | cut -f6 -d: | cut -f2 -d=);
		TH=$(echo $a | cut -f7 -d: | cut -f2 -d=);
		echo $ts $TL $TH;
	done > $DIR/ticks
fi

if [[ ! -f $DIR/update ]]; then
	echo regenerate $DIR/update
	cat $DIR/out | grep update | while read a; do
		t=$(echo $a | cut -f5 -d: | cut -f1 -d" ");
		ts=$(echo "scale=3; $t/1000.0" | bc)
		TL=$(echo $a | cut -f6 -d: | cut -f2 -d=);
		TH=$(echo $a | cut -f7 -d: | cut -f2 -d=);
		echo $ts $TL $TH;
	done > $DIR/update
fi

if [[ ! -f $DIR/bounds ]]; then
	echo regenerate $DIR/bounds
	cat $DIR/out | grep detection | grep calibrated | grep "t=" | grep -v ERR | while read a; do
		# compteur/detection.cpp:calibrated:24: t=201 ms:d=0:m=384:M=384
		t=$(echo $a | cut -f4 -d: | cut -f2 -d"=" | cut -f1 -d" ");
		ts=$(echo "scale=3; $t/1000.0" | bc)
		m=$(echo $a | cut -f6 -d: | cut -f2 -d=);
		M=$(echo $a | cut -f7 -d: | cut -f2 -d=);
		echo $ts $m $M;
	done > $DIR/bounds
fi


gnuplot ./simulation/tools/replay.gnuplot
killall feh || true
feh replay.png &


#!/usr/bin/env bash

set -e
# set -x

DIR=/tmp/replay/
INPUTFILE="$1"
#rm -Rf $DIR
mkdir -p $DIR

OUTPUTFILE=$DIR/out

function run() {
	if [[ "$1" = "simulate" ]]; then 
		find $DIR/ -type f -delete -print
		echo regenerate $DIR/out
		echo file: $ADCFILE
		if ! /tmp/build_pc/compteur/compteur $INPUTFILE > $OUTPUTFILE; then
			echo "program crash (ignore)"
		fi
	else
		cp $INPUTFILE $OUTPUTFILE
	fi
}

function split() {
	python3 recordings/tools/split-dump.py
}

function filter() {
	MARKER=$1
	echo regenerate $MARKER
	cat $OUTPUTFILE | grep "^\[" | grep $MARKER | split | grep $MARKER | cut -f2 -d: > $DIR/$MARKER
}

run # simulate

#for a in values ticks delta threshold; do
for a in value xalpha delta threshold ticked transmit; do
	filter $a
done

gnuplot ./recordings/tools/replay.gnuplot
killall feh || true
feh -g +50+100 replay.png &

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
		echo file: $INPUTFILE
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
	local MARKER=$1
	cat $OUTPUTFILE | grep "^\[" | grep "$MARKER:" | split | grep "^$MARKER:" | cut -b13- > $DIR/$MARKER
}

function plot() {
	for a in value xalpha delta threshold ticked deltamax; do
		filter $a
	done
	gnuplot ./recordings/tools/replay.gnuplot
}

function display() {
	killall feh || true
	feh -g +50+100 replay.png &
}

function loop() {
	killall feh || true
	feh --reload=5 -g +50+100 replay.png &
	while true; do
		run
		plot
		sleep 5;
		echo ok $(date -Is)
	done
}

run simulate
plot
display
# loop

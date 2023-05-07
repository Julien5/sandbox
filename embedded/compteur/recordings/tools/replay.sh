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
	if [[ "$1" = "truncate" ]]; then
		tail -8000 $OUTPUTFILE > /tmp/trunc
		mv /tmp/trunc $OUTPUTFILE
	fi
}

function split() {
	python3 recordings/tools/split-dump.py
}

function filter() {
	local MARKER=$1
	cat $OUTPUTFILE | grep "^\[" | grep " $MARKER:" | split | grep "^$MARKER:" | cut -b13- > $DIR/$MARKER
}

function plot() {
	for a in x xa d dmax T ticked; do
		filter $a
	done
	gnuplot ./recordings/tools/replay.gnuplot
}

function display() {
	killall feh || true
	feh -g +50+100 replay.png &
}

function simulate() {
	run simulate
	plot
	display
}

function replot() {
	run # truncate
	plot
	display
}

function monitor() {
	killall feh || true
	feh --reload=5 -g +50+100 replay.png &
	while true; do
		run truncate
		plot
		sleep 5;
		echo ok $(date -Is)
	done
}

replot
#monitor
#simulate

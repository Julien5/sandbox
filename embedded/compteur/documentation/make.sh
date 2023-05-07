#!/usr/bin/env bash

SCRIPTDIR=$(realpath $(dirname $0))

function extractdata() {
	if [ ! -d /tmp/replay ]; then
		cd $SCRIPTDIR/..
		./recordings/tools/replay.sh recordings/data/test-12/screen.log
	fi	
}

function plot_adc() {
	cd $SCRIPTDIR
	head -8000 /tmp/replay/x | tail -4000 | cut -b2- > /tmp/replay/adc
	gnuplot adc.gnuplot
}

function plot_sc() {
	cd $SCRIPTDIR
	gnuplot calibration.gnuplot
	gnuplot calibration2.gnuplot 
}

function main() {
	extractdata
	plot_adc
	plot_sc
}

main

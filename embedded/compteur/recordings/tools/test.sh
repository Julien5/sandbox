#!/usr/bin/env bash

#set -e
#set -x

function count() {
	rm -f /tmp/replay/out
	/tmp/build_pc/compteur/compteur simulation/data/$1/output.adc > /tmp/out
	cat /tmp/out | tr -d "\r" | grep counter | grep total | tail -1 | cut -f7 -d":"
}

function test() {
	echo -n replay $1:" "
	N1=$2
	N2=$(count $1)
	if [[ ! "$N1" = "$N2" ]]; then
		echo found "$N2" expected "$N1"
		return 1
	fi
	echo found "$N2" as expected "$N1"
	return 0;
}

test test-01 1
test test-02 1
test test-03 12
test test-04 14
test test-05 6

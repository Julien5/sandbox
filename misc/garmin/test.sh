#!/usr/bin/env bash

set -e
# set -x

function all() {
	find ~/projects/tracks/ -mindepth 1 -maxdepth 1 -type d | sort | while read d; do
		python3 src/readtracks.py $d;
	done 
}

function one() {
	python3 src/readtracks.py;
}

function main() {
	rm -Rf /tmp/plots
	if [ -z $1 ]; then
		one | tee test.out
		diff test.out test.reference || true 
	else
		$1
	fi
	#find /tmp/plots/ -name "*.gnuplot" -exec gnuplot "{}" \;
    #ristretto /tmp/plots/images/
}

main "$@"

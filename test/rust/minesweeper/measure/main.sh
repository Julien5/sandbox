#!/usr/bin/env bash

set -e
# set -x

SCRIPTDIR="$(dirname "$(realpath "$0")")"

function measure-worker() {
	TMPFILE2=$(mktemp /tmp/measure.XXX.txt)
	/usr/bin/time --output=${TMPFILE2} --format="%e s\n%M kB" \
				  $@ > /tmp/devnull
	cat ${TMPFILE2} | tr "\n" " "
	rm -f ${TMPFILE2}
}

function measure() {
	TMPFILE=$(mktemp /tmp/measure.XXX.txt)
	# 1>&2 echo exe: "$@"
	measure-worker "$@" &> ${TMPFILE}
	TIME=$(tail -1 ${TMPFILE} | cut -f1 -d" ")
	SPACE=$(tail -1 ${TMPFILE} | cut -f3 -d" ")
	printf "%ss %sk\n" "${TIME}" "${SPACE}"
	rm -f ${TMPFILE1}
}

function programs() {
	local n=$1
	shift
	case "$n" in
		1) echo ${CARGO_TARGET_DIR}/release/minesweeper baseline verbose
		   ;;
		2) echo ${CARGO_TARGET_DIR}/release/minesweeper candidate verbose
		   ;;
		3) echo /tmp/baseline-c margin verbose
		   ;;
		4) echo /tmp/baseline-c printable verbose
		   ;;
		5) echo /tmp/baseline-c orig verbose
		   ;;
		*) echo WTF
		   ;;
	esac
}

function collect() {
	for D in 2 4 8; do
		mkdir -p ~/delme/graph/${D}/
		for K in $(seq 1 4); do #  $(seq 6 4 22); do
			N=$((1024*K))
			B=$((N*N/D))
			printf "%5s|%10s" ${K} ${D} 
			for n in 1 2 3 4; do
				M=$(measure $(programs $n) ${N} ${B})
				TIME=$(echo ${M} | cut -f1 -d" " | tr -d s)
				SPACE=$(echo ${M} | cut -f2 -d" " | tr -d k)
				printf " |%5s|%10s" ${TIME} ${SPACE}
			done
			printf "\n"
		done | tee ~/delme/graph/${D}/K.txt
		echo
	done
}

function plot() {
	for D in 2 4 8; do
		cat ~/delme/graph/${D}/K.txt  > /tmp/time.dat
		gnuplot ${SCRIPTDIR}/time-array.gnuplot
		mv /tmp/time-array.png /tmp/time-array-${D}.png 
	done
}

function main() {
	# collect
	plot
}

main "$@"

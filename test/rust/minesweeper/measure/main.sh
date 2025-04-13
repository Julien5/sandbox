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
	#1>&2 echo exe: "$@"
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

function Ds() {
	echo 2 4 8
}

function Ls() {
	echo $(seq 1 4) $(seq 6 4 22)
}

function Ks() {
	echo 1 2 4 8 16 32 64
}

function collect() {
	K=1
	for D in $(Ds); do
		mkdir -p ~/delme/graph/${K}/${D}/
		for L in $(Ls); do
			N=$((1024*L))
			B=$((N*N/D))
			printf "%5s|%10s" ${L} ${D} 
			for n in 1 2 3 4; do
				M=$(measure $(programs $n) ${N} ${B})
				TIME=$(echo ${M} | cut -f1 -d" " | tr -d s)
				SPACE=$(echo ${M} | cut -f2 -d" " | tr -d k)
				printf " |%5s|%10s" ${TIME} ${SPACE}
			done
			printf "\n"
		done | tee ~/delme/graph/${K}/${D}/data.txt
		echo
	done
}

function collect-K() {
	CANDIDATE=$(programs 2);
	L=8;
	for D in 2; do
		for K in $(Ks); do
			N=$((1024*L))
			B=$((N*N/D))
			printf "%5s|%10s" ${K} ${D} 
			M=$(measure $CANDIDATE ${N} ${B} ${K})
			TIME=$(echo ${M} | cut -f1 -d" " | tr -d s)
			SPACE=$(echo ${M} | cut -f2 -d" " | tr -d k)
			printf " |%5s|%10s" ${TIME} ${SPACE}
			printf "\n"
		done | tee ~/delme/graph/${K}/${D}/measure.dat
		echo
	done
}

function plot() {
	K=1;
	for D in 2 4 8; do
		cat ~/delme/graph/${K}/${D}/measure.dat  > /tmp/measure.dat
		gnuplot ${SCRIPTDIR}/time-array.gnuplot
		mv /tmp/time-array.png /tmp/time-array-${D}.png 
	done

	D=4;
	for K in 2 4 8; do
		cat ~/delme/graph/${K}/${D}/measure.dat  > /tmp/measure.dat
		gnuplot ${SCRIPTDIR}/time-array.gnuplot
		mv /tmp/time-array.png /tmp/time-array-${D}.png 
	done
}

function main() {
	collect-K
	# plot
}

main "$@"

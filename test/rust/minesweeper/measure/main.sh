#!/usr/bin/env bash

set -e
# set -x

function measure() {
	TMPFILE2=$(mktemp /tmp/measure.XXX.txt)
	/usr/bin/time --output=${TMPFILE2} --format="%e s\n%M kB" \
				  $@ > /tmp/devnull
	cat ${TMPFILE2} | tr "\n" " "
	rm -f ${TMPFILE2}
}
function main() {
	TMPFILE=$(mktemp /tmp/measure.XXX.txt)
	measure "$@" &> ${TMPFILE}
	TIME=$(tail -1 ${TMPFILE} | cut -f1 -d" ")
	SPACE=$(tail -1 ${TMPFILE} | cut -f3 -d" ")
	printf "%ss %sk\n" "${TIME}" "${SPACE}"
	rm -f ${TMPFILE1}
}

main "$@"

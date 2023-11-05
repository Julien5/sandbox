#!/usr/bin/env bash

set -e
# set -x

function newestfile() {
	local dir="$1"
		if [ ! -d ${dir} ]; then
		return;
	fi
	find "${dir}" -type f -printf '%T@ %p\n' | sort -n | tail -1 | cut -f2- -d" "
}

function newfiles() {
	local srcdir="$1"
	local reference="$2"
	if [ ! -z "${reference}" ]; then
		find "${srcdir}" -anewer "${reference}" -type f
		return
	fi
	find "${srcdir}" -type f
}

function import() {
	local SRC=/media/julien/GARMIN/Garmin/GPX
	if [ ! -d $SRC ]; then
		echo no garmin mounted.
		return;
	fi
	local DST=$HOME/tracks/$(date +%Y.%m.%d);
	mkdir -p $DST
	cp -Rv $SRC/* $DST/
}

function createH() {
	#local HDIR=$HOME/tracks-H
	local HDIR=/tmp/tracks-H
	mkdir -p "${HDIR}"
	echo $HDIR
}

function hash() {
	local filename="$1"
	md5sum "${filename}" | cut -b-32
}

function import-to-H-file() {
	local filename="$1"
	base="$(basename "$filename")";
	dstdir=${HDIR}/$(hash "${filename}")
	mkdir -p "${dstdir}"/{gpx,meta}
   	dst="${dstdir}/gpx/original.gpx"
	realpath "${filename}" > "${dstdir}"/meta/origin
	if [ -f "${dst}" ]; then
		echo -n;
	else
		printf "importing %20s\n" "$filename"
		cp "${filename}" "${dst}"
	fi
}

function import-to-H() {
	local srcdir="$1"
	find ${srcdir} \( -name "Current.gpx" -o -name "Track*" \) -type f | while read filename; do
		import-to-H-file ${filename}
	done
}

function random_data() {
	od -vAn -N128 -tu2 < /dev/urandom
}

function simulate() {
	local TRACKSDIR=/tmp/tracks 
	mkdir -p ${TRACKSDIR}
	for n in $(seq 1 5); do
		k=$(find ${TRACKSDIR} -type f | wc -l)
		random_data > ${TRACKSDIR}/Track${k}.gpx
	done
	echo ${TRACKSDIR}
}

function main() {
	rm -Rf /tmp/tracks*
	#local TRACKSDIR=$HOME/tracks
	local TRACKSDIR=$(simulate)
	local HDIR=$(createH)
	#import-to-H $TRACKSDIR
	reference="$(newestfile ${HDIR})"
	newfiles "${TRACKSDIR}" "${reference}" | sort | while read filename; do
		import-to-H-file ${filename}
	done
	sleep 2
	simulate
	reference="$(newestfile ${HDIR})"
	newfiles "${TRACKSDIR}" "${reference}" | sort | while read filename; do
		import-to-H-file ${filename}
	done
}

main "$@"

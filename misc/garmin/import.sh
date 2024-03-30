#!/usr/bin/env bash

set -e
# set -x

function newestfile() {
	local dir="$1"
		if [ ! -d ${dir} ]; then
		return;
	fi
	find "${dir}" -type f -name "*.gpx" -printf '%T@ %p\n' | sort -n | tail -1 | cut -f2- -d" "
}

function newfiles() {
	local srcdir="$1"
	local reference="$2"
	if [ ! -z "${reference}" ]; then
		find "${srcdir}" -newer "${reference}" -type f
		return
	fi
	find "${srcdir}" -type f
}

function bad() {
	local filename="$1"
	shift
	local bfilename="$(basename "$filename")";
	local ext=$(echo "$bfilename" | tr "." "\n" | tail -1)
	if [[ ! "$ext" = "gpx" ]]; then
		printf "[%50s] bad type\n" "$filename"
		return 0;
	fi
	if [[ ! "$bfilename" = "Track_"*-*-*.gpx ]] && [[ ! "$bfilename" = "Current.gpx" ]]; then
		printf "[%50s] no record\n" "$filename"
		return 0;
	fi
	local ntime=$(xmllint --format "$filename" | grep time | wc -l);
	if (( ntime < 10 )); then
		printf "[%50s] too short %d\n" "$filename" "$ntime"
		return 0;
	fi
	return 1;
}

function import-from-gps() {
	local SRC=$1
	shift
	local DSTROOT=$1
	shift
	if [ ! -d $SRC ]; then
		echo no garmin mounted.
		return;
	fi
	local DST="${DSTROOT}"/$(date +%Y.%m.%d);
	mkdir -p $DST
	cp -Rv $SRC/* $DST/
}

function createH() {
	local HDIR=$HOME/projects/tracks
	#local HDIR=/tmp/tracks-H
	mkdir -p "${HDIR}"
	echo $HDIR
}

function hash() {
	local filename="$1"
	md5sum "${filename}" | cut -b-32
}

function import-to-H-file() {
	local filename="$1"
	if bad "$filename"; then
		return;
	fi
	local HDIR="$2"
	shift
	local dstdir=${HDIR}/$(hash "${filename}")
	mkdir -p "${dstdir}"/{gpx,meta}
   	dst="${dstdir}/gpx/origin.gpx"
	realpath "${filename}" > "${dstdir}"/meta/origin
	if [ -f "${dst}" ]; then
		echo -n;
	else
		printf "importing %20s\n" "$filename"
		cp "${filename}" "${dst}"
	fi
}

function import-new-files() {
	local SRCDIR="$1"
	shift
	local HDIR="$1"
	shift
	reference="$(newestfile "${HDIR}")"
	newfiles "${SRCDIR}" "${reference}" | sort | while read filename; do
		import-to-H-file "${filename}" "${HDIR}"
	done
}

function remove-files-from-GPS() {
	local GPSDIR="$1"
	shift
	# copy to tmp (just in case..)
	cp -Rf "$GPSDIR" /tmp/
	find "$GPSDIR" -type f -name "*.gpx" -print -delete 
}

function main() {
	# import-new-files test/D test/H
	# return
	local GPSDIR=/media/julien/GARMIN/Garmin/GPX
	local TRACKSDIR=$HOME/tracks
	import-from-gps "${GPSDIR}" "${TRACKSDIR}"
	remove-files-from-GPS ${GPSDIR}
	local HDIR=$(createH)
	import-new-files "${TRACKSDIR}" "${HDIR}"
}

main "$@"

# sudo mount -o remount,rw /dev/sdb /media/julien/GARMIN/
# find /media/julien/GARMIN/Garmin/GPX/ -type f -print -delete

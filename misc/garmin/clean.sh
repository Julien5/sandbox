#!/usr/bin/env bash

set -e
#set -x

TRACKSDIR=/home/julien/projects/tracks

function bad() {
	local DIRNAME="$1"
	shift
	if [[ $DIRNAME == *".git" ]]; then
		# skip that one
		return 1;
	fi
	local origin="$(cat "$a"/meta/origin)";
	local borigin="$(basename "$origin")";
	local ext=$(echo "$borigin" | tr "." "\n" | tail -1)
	if [[ ! "$ext" = "gpx" ]]; then
		printf "[%50s] bad type\n" "$borigin"
		return 0;
	fi
	if [[ ! "$borigin" = "Track_"* ]] && [[ ! "$borigin" = "Current.gpx" ]]; then
		printf "[%50s] no record\n" "$borigin"
		return 0;
	fi
	local ntime=$(xmllint --format "$a"/gpx/origin.gpx | grep time | wc -l);
	if (( ntime < 10 )); then
		printf "[%50s] too short %d\n" "$origin" "$ntime"
		return 0;
	fi
	return 1;
}

function cleanup() {
	cd $TRACKSDIR
	find $TRACKSDIR -maxdepth 1 -mindepth 1 -type d | while read a; do
		if bad "$a"; then
			git rm -r "$a"
			echo -n
		fi
	done
}

function main() {
	cleanup
}

main "$@"

#!/usr/bin/env bash

function make() {
	local input=$1
	shift
	local output=$1
	shift
	MAIN=/home/julien/projects/sandbox/misc/garmin/src/brevet/main.py
	python3 -u ${MAIN} ${input} --starttime 2024-05-11-11:00:00  --output ${output} 
}

rm /tmp/*.pdf /tmp/*.gpx
#make /home/julien/tours/brevets/2024/300/gpx/track.gpx /tmp/B300.gpx
#make /home/julien/tours/self/2024/05/gpx/studio/jura.gpx /tmp/jura.gpx
make /home/julien/tours/self/2024/05/gpx/studio/foret-noire.gpx /tmp/foret-noire.gpx
#make /home/julien/tours/self/2024/04/gpx/track-1.gpx /tmp/foret-noire-2.gpx

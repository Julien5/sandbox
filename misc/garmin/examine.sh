#!/usr/bin/env bash

for image in $(find /tmp/plots -type f -name "*.png" | sort); do
	echo $image
	feh --geometry +700+0 "$image" &
	dir=$(basename $image | tr "-" "/" | cut -f-5 -d.);
	gpxviewer readgpx2/$dir/auto/track.gpx &
	read a
	killall feh gpxviewer
done

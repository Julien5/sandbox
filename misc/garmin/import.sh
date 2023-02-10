#!/usr/bin/env bash

set -e
# set -x

SRC=/media/julien/GARMIN/Garmin/GPX

while [ ! -d $SRC ]; do
	echo no gpx dir
	sleep 1
done

DST=$HOME/tracks/$(date +%Y.%m.%d);
	
mkdir $DST
cp -Rv $SRC/* $DST/
# sudo mount -o remount,rw /dev/sdb /media/julien/GARMIN/

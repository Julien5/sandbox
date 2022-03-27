#!/usr/bin/env bash

set -e
# set -x

DIR=/tmp/replay/
ADCFILE="$1"
#rm -Rf $DIR
mkdir -p $DIR

if true; then #if [[ ! -f $DIR/out || "$1" = "build" ]]; then
	find $DIR/ -type f -delete -print
	echo regenerate $DIR/out
	echo file: $ADCFILE
	if ! /tmp/build_pc/compteur/compteur $ADCFILE > $DIR/out; then
		echo "program crash (ignore)"
	fi
fi

function filter() {
	MARKER=$1
	echo regenerate $MARKER
	# detection.cpp:tick:36: gnuplot:MARKER:1062.901978:492:522.880249
	cat $DIR/out | grep gnuplot:$MARKER | cut -f6- -d":" | grep -v "^$" | tr ":" " " > $DIR/$MARKER
}

for a in values ticks delta threshold; do
	filter $a
done
gnuplot ./recordings/tools/replay.gnuplot
killall feh || true
feh -g +50+100 replay.png &
echo -n found:" " 
cat $DIR/out | tr -d "\r" | grep counter | grep total | tail -1 | cut -f7 -d":"

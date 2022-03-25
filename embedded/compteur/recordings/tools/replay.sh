#!/usr/bin/env bash

set -e
#set -x

DIR=/tmp/replay/
ADCFILE="$1"
mkdir -p $DIR

if [[ ! -f $DIR/out || "$1" = "build" ]]; then
	find $DIR/ -type f -delete -print
	echo regenerate $DIR/out
	echo file: $ADCFILE
	/tmp/build_pc/compteur/compteur $ADCFILE > $DIR/out
fi

function filter() {
	MARKER=$1
	echo regenerate $MARKER
	cat $DIR/out | cut -f4- -d":" | grep -v "^$" | grep $MARKER |cut -f3- -d":" | tr ":" " " > $DIR/$MARKER
}

for a in values ticks delta variance; do
	filter $a
done
gnuplot ./recordings/tools/replay.gnuplot
killall feh || true
feh -g +50+100 replay.png &
echo -n found:" " 
cat $DIR/out | tr -d "\r" | grep counter | grep total | tail -1 | cut -f7 -d":"

#!/usr/bin/env bash

set -e
#set -x

DIR=/tmp/replay
mkdir -p /tmp/replay

if [[ ! -f $DIR/out || "$1" = "build" ]]; then
	find $DIR/ -type f -delete -print
	echo regenerate $DIR/out
	/tmp/build_pc/compteur/compteur > $DIR/out
fi

function filter() {
	MARKER=$1
	echo regenerate $MARKER
	cat $DIR/out | cut -f4- -d":" | grep -v "^$" | grep $MARKER |cut -f3- -d":" | tr ":" " " > $DIR/$MARKER
}

for a in values ticks update bounds errors; do
	filter $a
done
gnuplot ./simulation/tools/replay.gnuplot
killall feh || true
feh -g +50+100 replay.png &


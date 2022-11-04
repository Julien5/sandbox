#!/usr/bin/env bash

set -e
# set -x

DIR=/tmp/gpx
if [[ ! -d $DIR ]]; then
	mkdir -p ${DIR}/{good,bad}
	find ${DIR} -type f -delete
	find $HOME/tracks -name "*Track_*.gpx" | while read a; do
		echo reading $a;
		m=$(md5sum "$a" | cut -f1 -d" ")
		if ! xmllint --format "$a" > /dev/null; then
			cp "$a" ${DIR}/bad/${m}.gpx
		else
			cp "$a" ${DIR}/good/${m}.gpx
		fi;
	done
fi

python3 main.py

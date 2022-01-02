#!/usr/bin/env bash

set -e
DIR=/tmp/build_arduino/cmake/
mkdir -p $DIR/old/
if [[ -f $DIR/screenlog.0 ]]; then
	mv $DIR/screenlog.* $DIR/old/
fi
make -C /tmp/build_arduino/ flash-compteur monitor

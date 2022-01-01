#!/usr/bin/env bash

set -e
set -x

if [[ -z "$IDF_PATH" ]]; then
	echo IDF_PATH not defined
	exit 1
fi

if [[ -z "$1" ]]; then
	echo missing elf file argument
	exit 1
fi

ELFFILE=$1

$IDF_PATH/tools/idf_monitor.py --baud 74880 --port "/dev/ttyUSB0" $ELFFILE

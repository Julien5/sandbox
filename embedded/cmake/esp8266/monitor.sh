#!/usr/bin/env bash

set -e
# set -x

SCRIPTDIR=$(realpath $(dirname $0))
. $SCRIPTDIR/../catusb.sh

if [[ -z "$IDF_PATH" ]]; then
	echo IDF_PATH not defined
	exit 1
fi

if [[ -z "$1" ]]; then
	echo missing elf file argument
	exit 1
fi

ELFFILE=$1

ESPPORT="/dev/$(catusb | grep -i FT232 | cut -f1 -d":")"
echo using $ESPPORT
$IDF_PATH/tools/idf_monitor.py --baud 74880 --port $ESPPORT $ELFFILE

#!/usr/bin/env bash

set -e
# set -x

SCRIPTDIR=$(realpath $(dirname $0))
. $SCRIPTDIR/../catusb.sh

# assume esp is connected BEFORE arduino
PORTS=$(catusb | grep "HL-340 USB-Serial adapter" | cut -f1 -d: | sort)
NPORTS=$(echo $PORTS | tr " " "\n" | wc -l)
PORT=$(echo $PORTS | tr " " "\n" | tail -1)
if [ -z $PORT ]; then
	echo could not find port
	exit 1
fi
DEVICE="/dev/$PORT"
echo "flash using $DEVICE"
if [[ "$NPORT" = "1" ]]; then
	echo "(no esp connected)"
else
	echo "there seem to be an esp connected too"
	echo "=> esp must be connected before arduino"
fi

mkdir -p oldlogs
mv -v screenlog.* oldlogs/

screen -L $DEVICE 9600

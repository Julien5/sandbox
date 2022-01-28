#!/usr/bin/env bash

set -e
#set -x

SCRIPTDIR=$(realpath $(dirname $0))
. $SCRIPTDIR/../catusb.sh

if [[ -z "$1" ]]; then
	echo missing hex file argument
	exit 1
fi
FILENAME=$1

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

avrdude -q -V -p atmega328p -C /etc/avrdude.conf \
		-D -c arduino -b 57600 -P $DEVICE \
		-U flash:w:"$FILENAME":i

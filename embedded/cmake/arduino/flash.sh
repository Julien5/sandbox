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
PORT=$(catusb | grep "HL-340 USB-Serial adapter" | cut -f1 -d: | sort | tail -1)
DEVICE="/dev/$PORT"
echo using $DEVICE
avrdude -q -V -p atmega328p -C /etc/avrdude.conf \
		-D -c arduino -b 57600 -P $DEVICE \
		-U flash:w:"$FILENAME":i

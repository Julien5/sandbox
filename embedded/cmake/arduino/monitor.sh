#!/usr/bin/env bash

set -e
# set -x

SCRIPTDIR=$(realpath $(dirname $0))
. $SCRIPTDIR/../catusb.sh

# assume esp is connected BEFORE arduino
PORT=$(catusb | grep "HL-340 USB-Serial adapter" | cut -f1 -d: | sort | tail -1)
DEVICE="/dev/$PORT"
echo using $DEVICE

screen -L $DEVICE 9600

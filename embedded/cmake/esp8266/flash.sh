#!/usr/bin/env bash

set -e
#set -x

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
BINFILE=$ELFFILE.bin

# assume esp is connected BEFORE arduino
#PORT=$(catusb | grep "HL-340 USB-Serial adapter" | cut -f1 -d: | sort | head -1)
#PORT=$(catusb | grep "FT232" | cut -f1 -d: | sort | tail -1)
PORT=$(catusb | grep "QinHeng" | cut -f1 -d: | sort | tail -1)
if [ -z $PORT ]; then
	echo could not find port
	exit 1
fi
export DEVICE="/dev/$PORT"
echo using $DEVICE
export ESPBAUD=460800

# commands found building:
# pushd /opt/esp8266/esp8266-toolchain-espressif/ESP8266_RTOS_SDK/ && git clean -dfx && popd && rm -Rf /tmp/build && mkdir -p /tmp/build && cmake -S . -B /tmp/build && make -C /tmp/build flash VERBOSE=1

echo elf2image
python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp8266 elf2image --flash_mode dio --flash_freq 40m --flash_size 2MB --version=3 -o "$BINFILE" "$ELFFILE"

echo write_flash
python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp8266 -p $DEVICE -b 460800 write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x8000 /tmp/builds/esp8266/extern/partition_table/partition-table.bin 0x0 /tmp/builds/esp8266/extern/bootloader/bootloader.bin 0x10000 "$BINFILE"

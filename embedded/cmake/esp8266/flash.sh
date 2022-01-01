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
BINFILE=$ELFFILE.bin

# needed ?
export ESPPORT=/dev/ttyUSB0
export ESPBAUD=460800

# commands found building:
# pushd /opt/esp8266/esp8266-toolchain-espressif/ESP8266_RTOS_SDK/ && git clean -dfx && popd && rm -Rf /tmp/build && mkdir -p /tmp/build && cmake -S . -B /tmp/build && make -C /tmp/build flash VERBOSE=1

python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp8266 elf2image --flash_mode dio --flash_freq 40m --flash_size 2MB --version=3 -o "$BINFILE" "$ELFFILE"

python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp8266 -p /dev/ttyUSB0 -b 460800 write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x8000 /tmp/esp8266/core/partition_table/partition-table.bin 0x0 /tmp/esp8266/core/bootloader/bootloader.bin 0x10000 "$BINFILE"

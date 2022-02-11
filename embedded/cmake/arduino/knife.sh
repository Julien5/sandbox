#!/usr/bin/env bash

set -e
set -x

SCRIPTDIR=$(realpath $(dirname $0))
. $SCRIPTDIR/../catusb.sh

function ispport() {
	catusb | grep "8 Series USB xHCI HC" | cut -f1 -d: | sort | head -1
}

function dude() {
	avrdude -v -p atmega328p -c stk500 -P/dev/$(ispport) -b57600 $@
}

function burn.bootloader() {
	# erase, unlock (3F == FF for the lock bytes), set fuse 
	dude -e -Ulock:w:0xFF:m -Ulfuse:w:0xFF:m -Uhfuse:w:0xDE:m -Uefuse:w:0xFD:m 

	OPTIBOOTHEX=/tmp/optiboot/optiboot/bootloaders/optiboot/optiboot_atmega328.hex
	OPTIBOOTHEX=/tmp/arduino-1.8.19/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328.hex
	# rm -f $OPTIBOOTHEX
	if [ ! -f $OPTIBOOTHEX ]; then
		pushd /tmp/
		if [ ! -d optiboot ]; then
			git clone https://github.com/Optiboot/optiboot.git
		fi
		pushd optiboot/optiboot/bootloaders/optiboot
		make clean
		make AVR_FREQ=16000000L BAUD_RATE=115200 LED=B5 LED_START_FLASHES=5 LED_DATA_FLASH=1 atmega328
		popd
		popd
	fi
	# flash and lock bootloader section (CF==0F)
	dude -Uflash:w:$OPTIBOOTHEX:i -Ulock:w:0xCF:m
}

function burn.application() {
	avrdude -q -v -p atmega328p -D -c arduino -b115200 -P /dev/ttyUSB0 -U flash:w:/tmp/build_arduino/test/test.hex:i
}

# burn.bootloader
burn.application

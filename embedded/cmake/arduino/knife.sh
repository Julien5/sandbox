#!/usr/bin/env bash

set -e
#set -x

SCRIPTDIR=$(realpath $(dirname $0))
. $SCRIPTDIR/../catusb.sh

function ispport() {
	catusb | grep "8 Series USB xHCI HC" | cut -f1 -d: | sort | head -1
}

function ftdiport() {
	catusb | grep "Future Technology Devices International" | grep FT232 | cut -f1 -d: | sort | head -1
}

function dude() {
	avrdude -p atmega328p -c arduino -P/dev/$(ftdiport) -b38400 $@
}

function dude.isp() {
	avrdude -p atmega328p -c stk500 -P/dev/$(ispport) -b57600 $@
}

function burn.bootloader() {
	# erase, unlock (3F == FF for the lock bytes), set fuse 
	dude.isp -e -Ulock:w:0xFF:m -Ulfuse:w:0xDD:m -Uhfuse:w:0xDE:m -Uefuse:w:0xFD:m 

	OPTIBOOTHEX=/tmp/optiboot/optiboot/bootloaders/optiboot/optiboot_atmega328.hex
	# OPTIBOOTHEX=/tmp/arduino-1.8.19/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328.hex
	rm -f $OPTIBOOTHEX
	if [ ! -f $OPTIBOOTHEX ]; then
		pushd /tmp/
		if [ ! -d optiboot ]; then
			git clone https://github.com/Optiboot/optiboot.git
		fi
		pushd optiboot/optiboot/bootloaders/optiboot
		make clean
		make AVR_FREQ=4000000L BAUD_RATE=38400 LED=B5 LED_START_FLASHES=5 LED_DATA_FLASH=1 atmega328
		sleep 1
		popd
		popd
	fi
	# flash and lock bootloader section (CF==0F)
	dude.isp -Uflash:w:$OPTIBOOTHEX:i -Ulock:w:0xCF:m
}

function burn.application() {
	dude -U flash:w:/tmp/build_arduino/test/test.hex:i
}

burn.bootloader
burn.application

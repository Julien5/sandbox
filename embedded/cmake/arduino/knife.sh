#!/usr/bin/env bash

set -e
set -x

SCRIPTDIR=$(realpath $(dirname $0))
. $SCRIPTDIR/../catusb.sh

function ispport() {
	FILTER="head -1"
	if [ ! -z "$1" ]; then
		FILTER="$1"
	fi
	catusb | grep "8 Series USB xHCI HC" | cut -f1 -d: | sort | $FILTER
}

function ftdiport() {
	catusb | grep "Future Technology Devices International" | grep FT232 | cut -f1 -d: | sort | head -1
}

function arduinoport() {
	catusb | grep HL-340 | cut -f1 -d: | sort | tail -1
}

function dude.arduino() {
	# avrdude -p atmega328p -c arduino -P/dev/$(ftdiport) -b38400 $@
	avrdude -p atmega328p -c arduino -P/dev/$(arduinoport) -b57600 $@
}

function dude.isp() {
	# specifying a baud rate seems useless. In doubt try -b57600.
	avrdude -p atmega328p -c stk500 -P/dev/$(ispport) -B 0.5 $@ 
}

function burn.fuse() {
	# erase, unlock (3F == FF for the lock bytes), set fuse
	# lfuse:w:0xDD:m -> external crystal 3-8 mhz. (16mhz:0xFF, internal:0xE2)
	# efuse:w:0xFF:m -> BOD disabled (otherwise programming per ftdi fails.)
	dude.isp -e -Ulfuse:w:0xE2:m -Uhfuse:w:0xDE:m -Uefuse:w:0xFF:m 
}

function burn.bootloader() {
	# erase, unlock (3F == FF for the lock bytes), set fuse
	# lfuse:w:0xDD:m -> external crystal 3-8 mhz. (16mhz:0xFF, internal:0xE2)
	# efuse:w:0xFF:m -> BOD disabled (otherwise programming per ftdi fails.)
	dude.isp -e -Ulock:w:0xFF:m

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
		make AVR_FREQ=8000000L BAUD_RATE=38400 LED=B5 LED_START_FLASHES=5 LED_DATA_FLASH=1 atmega328
		sleep 1
		popd
		popd
	fi
	# flash and lock bootloader section (CF==0F)
	dude.isp -Uflash:w:$OPTIBOOTHEX:i -Ulock:w:0xCF:m
}

function burn.application() {
	dude.isp -U flash:w:$1:i
}

function monitor() {
	mkdir -p oldlogs
	find -name "*.log" -exec mv "{}" oldlogs/ \;
	screen  -Logfile log-$(date +%Y.%m.%d.%H.%M.%S).log  -L /dev/$(ispport "tail -1") 9600
}

function burn.test {
	burn.fuse
	burn.application /tmp/build_arduino/test/test.hex 
}

function help {
	echo knife.sh burn.test
	echo knife.sh monitor
	echo knife.sh burn.application foo.hex
	echo knife.sh ispport
	echo knife.sh ispport "tail -1"
}

"$@"

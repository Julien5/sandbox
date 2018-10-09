#!/bin/bash

set -e

optiboot_dir="../../optiboot/optiboot/bootloaders/optiboot"

# 1. compile bootloader
pushd "$optiboot_dir"
make clean
make AVR_FREQ=4000000L BAUD_RATE=38400 LED_START_FLASHES=2 atmega328
ls *.hex
popd

# 2. turn the arduino uno board into isp

pushd isp
make
echo "connect arduino uno board"
echo "remove 10uF capacitor between reset and GND of programmer"
read ok
make upload
echo "arduino as isp."
popd

# 3. set fuses and burn bootloader
# note: make ispload returns 0 even when the target is disconnected => not reliable.

echo "please insert 10uF capacitor between reset and GND of programmer"
read ok
echo "fusing the target to disable BODLEVELS"
avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -e -Ulock:w:0xFF:m -Uefuse:w:0xFF:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m
echo "burning bootloader"
avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -Uflash:w:"$optiboot_dir"/optiboot_atmega328.hex:i -Ulock:w:0xCF:m 

# 4. test app

pushd blink
make
avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -Uflash:w:./build-uno/blink.hex:i 
popd

echo "OK?"
read ok

if [ ! -z "$1" ] && [ -f "$1" ]; then
	echo press enter to upload "$1"
	read ok
	avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -Uflash:w:$1:i
fi




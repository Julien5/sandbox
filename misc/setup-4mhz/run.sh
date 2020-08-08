#!/usr/bin/env bash

echo "this is a big mess"
echo "don't use it."
echo "use the arduino ide."
exit 1

set -e

if [ -z "$1" ]; then
    echo "run.sh build isp burn"
    exit 1
fi

optiboot_dir="../../optiboot/optiboot/bootloaders/optiboot"

# 1. compile bootloader
if [ "$1" = "build" ]; then
    pushd "$optiboot_dir"
    make clean
    find . -name "*.hex" -print -delete
    make AVR_FREQ=16000000L BAUD_RATE=38400 LED_START_FLASHES=2 atmega328
    ls *.hex
    popd
    exit 0;
fi

# 2. turn the arduino uno board into isp
if [ "$1" = "isp" ]; then
    pushd isp
    make
    echo "connect arduino uno board"
    echo "remove 10uF capacitor between reset and GND of programmer"
    read ok
    make upload
    echo "arduino as isp."
    popd
    exit 0;
fi

# 3. set fuses and burn bootloader
# note: make ispload returns 0 even when the target is disconnected => not reliable.
if [ "$1" = "burn" ]; then
    echo "please insert 10uF capacitor between reset and GND of programmer"
    read ok
    echo "fusing the target to disable BODLEVELS"

    set -x
    
    avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -e -Ulock:w:0xFF:m -Uefuse:w:0xFD:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m
    avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -Uflash:w:/home/julien/arduino/arduino-1.8.6/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328.hex:i -Ulock:w:0xCF:m 


    
    #avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -e -Ulock:w:0xFF:m -Uefuse:w:0xFF:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m
    echo "burning bootloader"
    defaulthex="$HOME/arduino/arduino-1.8.6/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328.hex"
    #avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -Uflash:w:$defaulthex:i  -Ulock:w:0xCF:m
    #avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -Uflash:w:"$optiboot_dir"/optiboot_atmega328.hex:i -Ulock:w:0xCF:m 

    # 4. test app
    echo "upload test blink."
    pushd blink
    make
    avrdude -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -Uflash:w:./build-uno/blink.hex:i 
    popd
fi




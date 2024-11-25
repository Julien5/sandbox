#!/usr/bin/env bash

. $HOME/.bashrc 

set -e
set -x

SCRIPTDIR=$(dirname $(realpath $0))
cd ${SCRIPTDIR}

dev.avr

BUILDDIR=/tmp/builds/avr
SOURCEDIR=$(realpath ..)
TOOLCHAIN=${SOURCEDIR}/cmake/arduino/toolchain.cmake

# rm -Rf /tmp/builds
cmake -B ${BUILDDIR} -S ${SOURCEDIR} --toolchain ${TOOLCHAIN} -DCMAKE_EXPORT_COMPILE_COMMANDS=1
VERBOSE=1 make -C ${BUILDDIR}

avrdude -p atmega328p -P /dev/ttyUSB0 -c arduino -b 57600 -D -U flash:w:/tmp/builds/avr/test-avr/test-avr.hex:i

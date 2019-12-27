#/!/usr/bin/env bash

# build the toolchain
#
# requires
# sudo apt-get install make autoconf automake libtool gcc g++ gperf \
#    flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python python-serial \
#    sed git unzip bash help2man wget bzip2 libtool-bin
#
# https://dl.espressif.com/dl/xtensa-lx106-elf-linux32-1.22.0-100-ge567ec7-5.2.0.tar.gz
# in ~/Downloads/xtensa-lx106-elf-linux32-1.22.0-100-ge567ec7-5.2.0.tar.gz
#
# https://github.com/pfalcon/esp-open-sdk/
# - git clone 
# - make toolchain esptool libhal STANDALONE=n
# (long time)...
#
# https://github.com/SuperHouse/esp-open-rtos
# - just git clone

set -e

SOURCEDIR=$HOME/tmp
DESTINATIONDIR=$HOME/projects/esp8266-toolchain

# rm -Rf $DESTINATIONDIR

mkdir -p $DESTINATIONDIR/compiler
pushd $DESTINATIONDIR/compiler
if [[ ! -d xtensa-lx106-elf ]]; then
    echo uncompress...
    tar zxf ~/Downloads/xtensa-lx106-elf-linux32-1.22.0-100-ge567ec7-5.2.0.tar.gz
fi
popd

# copy missing (update)
function cpm() {
    SRC=$1
    l=$(echo $SRC | tr "/" "\n" | grep -n esp-open-sdk | cut -f1 -d":")
    n=$((l+1))
    DST=$2/$(echo $SRC | cut -f$n- -d"/")
    if [[ -e "$DST" ]]; then
	echo skip $SRC;
	return;
    fi
    D=$(dirname $DST)
    if [[ ! -d $D ]]; then
	mkdir -p $D
    fi
    echo install $(basename $SRC)
    cp $SRC $DST;
}

# copy libhdal headers (?)
SYSROOT=xtensa-lx106-elf/xtensa-lx106-elf/sysroot
find $SOURCEDIR/esp-open-sdk/$SYSROOT/usr/include/xtensa/ -type f | \
    (while read a; do
	 cpm "$a" $DESTINATIONDIR/compiler \;
     done)

cpm $SOURCEDIR/esp-open-sdk/$SYSROOT/usr/lib/libhal.a $DESTINATIONDIR/compiler
cp $SOURCEDIR/esp-open-sdk/esptool/esptool.py $DESTINATIONDIR/compiler/xtensa-lx106-elf/bin/

if [[ ! -d $DESTINATIONDIR/esp-open-rtos ]]; then
    echo install esp-open-rtos
    cp -Rf $SOURCEDIR/esp-open-rtos $DESTINATIONDIR/
fi

# build example.
rm -Rf /tmp/blink
cp -Rf ../blink /tmp/blink
PATH=$PATH:$DESTINATIONDIR/compiler/xtensa-lx106-elf/bin/
# $SOURCEDIR/esp-open-rtos/examples/blink/
cd /tmp/blink 
make clean
make -j3
make flash ESPPORT=/dev/ttyUSB0

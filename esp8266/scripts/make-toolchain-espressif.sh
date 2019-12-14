#!/usr/bin/env bash

# TMPDIR=$HOME/tmp/toolchain-espressif
TMPDIR=$HOME/projects/esp8266-toolchain-espressif
mkdir -p $TMPDIR
cd $TMPDIR

mkdir -p compiler
pushd compiler
TGZ=xtensa-lx106-elf-linux32-1.22.0-100-ge567ec7-5.2.0.tar.gz
if [[ ! -e $TGZ ]]; then
    wget https://dl.espressif.com/dl/$TGZ
fi
if [[ ! -d xtensa-lx106-elf ]]; then
    tar zxvf $TGZ
fi
popd

if [[ ! -d ESP8266_RTOS_SDK ]]; then
    git clone https://github.com/espressif/ESP8266_RTOS_SDK.git
fi

sudo apt-get install python-click
sudo apt-get install python-pyelftools


#!/usr/bin/env bash

function build0() {
    rm -Rf /tmp/build0 build0.log
    make -f Makefile.esp-open-rtos V=1 &> build0.log
}

function build() {
    source ~/.profile;
    dev.esp8266.open.rtos 
    rm -Rf /tmp/build build.log
    make -f Makefile.esp8266 &> build.log
}

function rebuild() {
    build0;
    build;
}

function size() {
    stat $a | grep Size | cut -f4 -d" "
}

function check_sizes() {
    find /tmp/build0 -type f | while read a; do
	o="$a"
	n="${a/build0/build}"
	if [[ $(size $o) -ne $(size $n) ]]; then
	    echo $o
	    ls -l $o $n
	fi
    done
}

rebuild
find /tmp/ -name "blink.o" -exec ls -l "{}" \; 2>/dev/null
find /tmp/ -name "foo.o" -exec ls -l "{}" \; 2>/dev/null

echo
echo raw
ls -l /tmp/build/esp8266/min/min.esp8266 /tmp/build0/esp8266/core/blink.out

echo
echo stripped
ls -l min.bin firmware/blink.bin


grep min.esp8266 build.log         | head -1 | tr " " "\n" | sort > flags.log
grep firmware/blink.bin build0.log | head -1 | tr " " "\n" | sort > flags0.log
# meld flags*


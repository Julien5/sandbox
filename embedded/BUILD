#!/usr/bin/env bash

for a in pc arduino esp8266; do BUILDDIR=/tmp/build_$a; mkdir -p $BUILDDIR && cmake -DCMAKE_TOOLCHAIN_FILE=~/projects/sandbox/embedded/cmake/$a/toolchain.cmake -S ~/projects/sandbox/embedded/ -B $BUILDDIR && make -C $BUILDDIR; done

cp -v /tmp/build_pc/compile_commands.json .

#!/usr/bin/env bash

source ~/.profile
dev.esp8266

for a in pc arduino esp8266; do
	BUILDDIR=/tmp/build_$a;
	mkdir -p $BUILDDIR && cmake -DCMAKE_TOOLCHAIN_FILE=~/projects/sandbox/embedded/cmake/$a/toolchain.cmake -S ~/projects/sandbox/embedded/ -B $BUILDDIR && make -j3 -C $BUILDDIR;
done

cp -v /tmp/build_pc/compile_commands.json ~/projects/sandbox/embedded/

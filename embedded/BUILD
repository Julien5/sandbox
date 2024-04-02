#!/usr/bin/env bash

set -e
# set -x

function init() {
	source ~/.profile
}

function compile() {
	a=$1
	shift
	if [[ $a == esp8266 ]]; then
		dev.esp8266
	fi
	BUILDDIR=/tmp/build_$a;
	mkdir -p $BUILDDIR
	cmake -DCMAKE_TOOLCHAIN_FILE=$HOME/projects/sandbox/embedded/cmake/$a/toolchain.cmake \
		  -S $HOME/projects/sandbox/embedded/ \
		  -B $BUILDDIR
	make VERBOSE=1 -C $BUILDDIR 
	if [[ $a == pc ]]; then
		cp -v /tmp/build_pc/compile_commands.json ~/projects/sandbox/embedded/
	fi
	
}

function main() {
	local target=
	if (( "$#" > 0 )); then
		target=$1
		shift
	fi
	if [[ -z $target ]]; then
		for a in pc arduino esp8266; do
			compile $a;
		done
	else
		compile $target
	fi
	echo done
}

init
main "$@"
echo done

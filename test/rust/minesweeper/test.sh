#!/usr/bin/env bash

set -e
# set -x

function build() {
	source ~/.profile
	dev.rust
	cargo build --release 
	find . \( -name "*.cpp" -o -name "*.rs" \) -exec touch "{}" \;
	echo rust build
 	time cargo --verbose build --release
	echo C++ build
	rm -Rf /tmp/build/
	mkdir -p /tmp/build
	cmake -B /tmp/build -S C -DCMAKE_BUILD_TYPE=Release
	time VERBOSE=1 make -j8 -C /tmp/build
}

function gen() {
	local L=$1
	shift
	local D=$1
	local N=$((128*L));
	local B=$((N*N/D));
	DIR=/tmp/minesweeper-test/${L}/${D}
	mkdir -p ${DIR}
	cargo run -- candidate verbose $N $B > ${DIR}/rust.candidate
	cargo run -- baseline verbose $N $B > ${DIR}/rust.baseline
	/tmp/build/minesweeper-cpp baseline verbose $N $B > ${DIR}/cpp.baseline
	/tmp/build/minesweeper-cpp margin verbose $N $B > ${DIR}/cpp.margin
}

function main() {
	# build
	rm -Rf /tmp/minesweeper-test
	export TEST=1
	for L in 1 3 8; do
		for D in 2 4 8; do
			printf "L=%5d D=%5d\n" $L $D
			gen $L $D
		done
	done
	find /tmp/minesweeper-test/ -type f | sort | while read a; do md5sum $a; done
}

main "$@"

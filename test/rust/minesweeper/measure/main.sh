#!/usr/bin/env bash

set -e
set -x

function main() {
	EXE=/home/julien/delme/rust-targets/debug/minesweeper
	/usr/bin/time --output=/tmp/output.txt --format="%e s\n%M kB" \
				  ${EXE} $@
	cat /tmp/output.txt 
}

main "$@"

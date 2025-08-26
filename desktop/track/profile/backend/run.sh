#!/usr/bin/env bash

set -e
# set -x

function pdf() {
	echo "args:"$@
	file=data/blackforest.gpx
	if [ -f "$1" ]; then
	   file=$1
	   shift
	fi
	echo make pdf
	cargo run -- \
		  --output-directory /tmp/ \
		  --debug true \
		  --interval-length 180 \
		  "$@" \
		  ${file}
	TYPST=/opt/typst/typst-x86_64-unknown-linux-musl/typst
	${TYPST} compile /tmp/document.typst
	echo xdg-open /tmp/document.pdf 
}

function main() {
	export RUST_BACKTRACE=1
	rm -f /tmp/*.{svg,pdf,gpx,typ} /tmp/document.*
	pdf "$@"
	# exp
	ls /tmp/*.svg
	ls /tmp/*.pdf
}


main "$@"

#!/usr/bin/env bash

set -e
set -x

function main() {
	/usr/bin/time --output=/tmp/output.txt --format="%e s\n%M kB" \
				  $@ > /dev/null
	cat /tmp/output.txt 
}

main "$@"

#!/usr/bin/env bash

set -e
#set -x

EXE=/tmp/build_pc/app
rm -f res.dat
for b in $(seq 100 1000 100000); do
	echo $b
	#/usr/bin/time --output=foo.txt --format="%e\n%M" \
	#			  $EXE 1000 1000 ${b} &> /dev/null;
	/usr/bin/time --output=foo.txt --format="%e\n%M" \
				  $EXE $b $b 1000 &> /dev/null;
	elapsed=$(cat foo.txt | head -1);
	memory=$(cat foo.txt | tail -1);
	echo $b $elapsed $memory >> res.dat
	sleep 0.1;
done

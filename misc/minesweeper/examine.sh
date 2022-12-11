#!/usr/bin/env bash

set -e
#set -x

EXE1=/tmp/build_pc/jbo/jbo
EXE2=/tmp/build_pc/msa/msa
rm -f /tmp/{space,time}.dat
for b in $(seq 0 10 200); do
	echo $b $EXE1
	/usr/bin/time --output=/tmp/1.txt --format="%e\n%M" \
				  $EXE1 1000 1000 ${b} &> /dev/null;
	echo $b $EXE2
	/usr/bin/time --output=/tmp/2.txt --format="%e\n%M" \
				  $EXE2 1000 1000 ${b} &> /dev/null;
	echo OK
	(
		printf "%s " $b;
		for r in /tmp/1.txt /tmp/2.txt; do
			elapsed=$(cat $r | head -1);
			printf "%s " $elapsed 
		done
		echo
	) >> /tmp/space.dat
	echo OK
	(
		printf "%s " $b;
		for r in /tmp/1.txt /tmp/2.txt; do
			elapsed=$(cat $r | tail -1);
			printf "%s " $elapsed 
		done
		echo
	) >> /tmp/time.dat
	# sleep 0.1;
done

gnuplot space.gnuplot
gnuplot time.gnuplot
ristretto space.png time.png

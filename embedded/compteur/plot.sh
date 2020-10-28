#!/usr/bin/env bash

set -e
set -x

sqlite3 ../../server/sqlite.db ".dump" > /tmp/dump;
cat /tmp/dump | grep adc | cut -f4 -d"'" | python3 chunkify.py > /tmp/adc0.csv
for N in $(seq 100 1000 $(wc -l /tmp/adc0.csv | cut -f1 -d" ")); do
	cat /tmp/adc0.csv | head --lines=$N | tail -5000 > /tmp/adc.csv;
	gnuplot adc.gnuplot;
	killall feh || true
	feh adc.png 
done

cat /tmp/dump | grep histo | cut -f4 -d"'" > histogram.packed.hex;
python3 ../../server/deserialization/histogram.py 

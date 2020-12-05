#!/usr/bin/env bash

set -e
set -x

#sqlite3 ../../server/sqlite.db ".dump" > /tmp/dump;
#cat /tmp/dump | grep adc | cut -f4 -d"'" | python3 chunkify.py > /tmp/adc.csv
gnuplot adc.gnuplot;
killall feh || true
feh adc.png 

cat /tmp/dump | grep histo | cut -f4 -d"'" > histogram.packed.hex;
cat /tmp/dump | grep status | cut -f4 -d"'" > status.packed.hex;
#python3 ../../server/deserialization/histogram.py 

#!/usr/bin/env bash

set -e
set -x

cat /tmp/dump | grep histo | cut -f4 -d"'" > /tmp/histogram.packed.hex;
#python3 ../../server/deserialization/histogram.py

cat /tmp/dump | grep status | cut -f4 -d"'" > /tmp/status.packed.hex;
python3 ../../server/deserialization/status.py  > /tmp/status.csv

gnuplot adc.gnuplot;
killall feh || true
feh adc.png 

#/usr/bin/env bash

set -e
set -x


function reduce() {
	tail -5000 $1 > /tmp/reduce
	mv /tmp/reduce $1
}

grep status screenlog.0 | cut -f2-5 -d" " > /tmp/status.csv

grep update screenlog.0 | cut -f2 -d: > /tmp/data.csv
python3 add-time-column.py 200 > /tmp/addline
mv /tmp/addline /tmp/analog_input.csv

grep histo screenlog.0  | cut -f2 -d: > /tmp/histogram.out
reduce /tmp/histogram.out
python3 histogramify.py /tmp/histogram.out > /tmp/histogram.dat;

reduce /tmp/analog_input.csv
reduce /tmp/status.csv

gnuplot graphs.gnuplot
#feh /tmp/graphs.png &

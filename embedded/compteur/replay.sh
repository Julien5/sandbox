#!/usr/bin/env bash

set -e
set -x

if [[ ! -f /tmp/dump ]]; then
	sqlite3 ../../server/sqlite.db ".dump" > /tmp/dump;
fi

cat /tmp/dump | grep adc | cut -f4 -d"'" | python3 chunkify.py > /tmp/data.csv
python3 add-time-column.py 200 > /tmp/addline
mv /tmp/addline /tmp/analog_input.csv
rm /tmp/data.csv

make

./compteur.pc > /tmp/compteur.out

grep status /tmp/compteur.out | cut -f3-6 -d" " > /tmp/status.csv


cat /tmp/compteur.out | grep histogra | grep print:60 | cut -f5 -d":" > /tmp/histogram.out
python3 histogramify.py /tmp/histogram.out > /tmp/histogram.dat;
gnuplot graphs.gnuplot
feh /tmp/graphs.png &

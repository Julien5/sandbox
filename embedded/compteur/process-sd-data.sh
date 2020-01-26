#!/usr/bin/env bash

set -e

D=sd-data/4
#D=/tmp
mkdir -p $D/hex
echo convert hex
find $D/ -iname "*.bin" | while read a; do xxd $a | cut -f2 -d: | cut -b1-40 | tr -d " " | tr -d "\n"; echo; done | tr "[:lower:]" "[:upper:]" > $D/hex/data
echo copy
cp $D/hex/data /tmp/irdata
echo convert csv
rm data.csv
python3 readirdata.py 
head data.csv 
gnuplot readirdata.gnuplot
xdg-open ir.png 

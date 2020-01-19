#!/usr/bin/env bash

set -e
set -x

#scp julien@pi:/tmp/irdata /tmp
python3 readirdata.py
gnuplot readirdata.gnuplot
#xdg-open ir.png 

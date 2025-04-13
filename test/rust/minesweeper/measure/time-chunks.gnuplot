set terminal pngcairo size 800,800 
set output '/tmp/time-chunks.png'
set datafile separator "|"

set xlabel "chunks count"
set ylabel "time [s]"

set yrange [0:6]

plot '/tmp/measure.dat' using 1:($3) with linespoints linestyle 1 title "rs-margin"

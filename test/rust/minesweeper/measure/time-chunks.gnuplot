set terminal pngcairo size 800,400 
set output '/tmp/time-chunks.png'
set datafile separator "|"

set xlabel "chunks count"
set ylabel "time [s]"

set yrange [0:6]
set xrange [0:18]

plot '/tmp/measure.dat' using 1:($3) with linespoints linestyle 7 title "rs-parallel"

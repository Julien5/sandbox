set terminal pngcairo size 800,400 
set output '/tmp/time-chunks.png'
set datafile separator "|"

set xlabel "chunks count K"
set ylabel "time [s]"

set yrange [0:6]
set xrange [0:18]
set grid

plot '/tmp/measure.dat' using 1:($3) with linespoints linestyle 7 title "rs-parallel"

P(x) = (x/1024)
set yrange [0:1000]
set output '/tmp/space-chunks.png'
set ylabel "memory [kB]"
plot '/tmp/measure.dat' using 1:(P($4)) with linespoints linestyle 7 title "rs-parallel"

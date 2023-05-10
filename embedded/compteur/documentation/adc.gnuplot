set terminal pngcairo size 1200,600 enhanced 
set output 'adc.png'

set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 8 pi -1 ps 1.5
set style line 2 lc rgb '#ff0000' lt 1 lw 2 pt 8 pi -1 ps 1.5
set pointintervalbox 3
set grid;

set xrange [200:900]
set yrange [-50:350]
set autoscale fix
set xlabel "time [seconds]"
set ylabel "adc output"

set arrow from 200,100 to 500,100 nohead
set arrow from 505,-20 to 610,-20 nohead
set arrow from 615,100 to 900,100 nohead

set label 1 at 310,110 'no mark in front of the detector' center front
set label 2 at 560,-10 'mark' center front
set label 3 at 750,110 'no mark in front of the detector' center front

plot '/tmp/replay/adc' using 1:2 title 'x' with lines ls 1


set terminal pngcairo size 1200,600 enhanced font 'Courier,10'
set output 'replay.png'
set multiplot layout 2,1 rowsfirst

set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 8 pi -1 ps 1.5
set style line 2 lc rgb '#ff0000' lt 1 lw 2 pt 8 pi -1 ps 1.5
set pointintervalbox 3
set grid;

#set yrange [70:100]
set xrange [000:4000]
set autoscale fix
plot '/tmp/replay/x' using 1:2 title 'adc' with lines ls 1, \
     '/tmp/replay/xa' using 1:2 title 'x_{a}}' with lines ls 2, \
	 '/tmp/replay/ticked' using 1:2 title 'ticks' with points ls 5

#set yrange [70:100]
plot '/tmp/replay/d' using 1:2 title 'delta' with lines ls 1, \
	 '/tmp/replay/T' using 1:2 title 'threshold' with lines ls 2, \
	 '/tmp/replay/dmax' using 1:2 title 'deltamax' with lines ls 3
set terminal pngcairo size 1800,800 enhanced font 'Courier,10'
set output 'replay.png'
set multiplot layout 2,1 rowsfirst

set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 8 pi -1 ps 1.5
set pointintervalbox 3
set grid;

#set yrange [70:100]
#set xrange [0:100]
#set autoscale fix
plot '/tmp/replay/values' using 1:2 title 'adc' with lines ls 1, \
     '/tmp/replay/values' using 1:3 title 'x_alpha' with lines ls 6, \
	 '/tmp/replay/ticks' using 1:2 title 'ticks' with points ls 5

plot '/tmp/replay/delta' using 1:2 title 'delta' with lines ls 1

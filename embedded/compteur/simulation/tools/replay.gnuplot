set terminal pngcairo size 1800,800 enhanced font 'Courier,10'
set output 'replay.png'
set multiplot layout 2,1 rowsfirst

set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 8 pi -1 ps 1.5
set pointintervalbox 3
set grid;

#set yrange [70:100]
set xrange [0:400]
#set autoscale fix
plot '/tmp/replay/values' title 'adc' with lines ls 1, \
	 '/tmp/replay/update' using 1:2 title 'T_L' with linespoints ls 3, \
	 '/tmp/replay/update' using 1:3 title 'T_H' with linespoints ls 4, \
	 '/tmp/replay/bounds' using 1:2 title 'm' with lines ls 5, \
	 '/tmp/replay/bounds' using 1:3 title 'M' with lines ls 6, \
	 '/tmp/replay/ticks' using 1:2 title 'ticks' with points ls 5

set size 1,.25
set origin 0,0.25
#set yrange [90:100]
plot '/tmp/replay/features' using 1:3 title 'p95' with points ls 2
#plot '/tmp/replay/errors' using 1:($2==1):($2 == 1 ? 0x0000ff : 0xffffff) title 'd5' with boxes lc rgb variable, \
#	 '/tmp/replay/errors' using 1:($2==2):($2 == 2 ? 0x00ff00 : 0xffffff) title 'd10' with boxes lc rgb variable

set size 1,.25
set origin 0,0.0
set yrange [0:10]
plot '/tmp/replay/features' using 1:2 title 'p5' with points ls 1

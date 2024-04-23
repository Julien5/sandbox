set terminal pngcairo size 1200,600 enhanced 
set output 'speed.png'

set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 8 pi -1 ps 1.5
set style line 2 lc rgb '#ff0000' lt 1 lw 2 pt 8 pi -1 ps 1.5
set pointintervalbox 3
set grid;

set autoscale fix
set xlabel "time"
set ylabel "speed [kmh]"

set yrange [0:50]

set label 1 'distance: {km} km' at 100,45
set label 2 'time: {time}' at 100,35
set label 3 'start: {start}' at 100,40
set title 'name'

plot '/tmp/plot.csv' using 2:3 with lines ls 1 title "speed", \
	 '/tmp/plot.csv' using 2:(NaN):xtic(1) every 200 notitle



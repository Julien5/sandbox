set terminal pngcairo size 1800,600 enhanced font 'Courier,10'
set output 'adc.png'

# plot "data.csv" with lines linestyle 1

set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5
set style line 2 lc rgb 'red' pt 7 ps 0.2 lt 1 lw 2
#set pointintervalbox 3
set grid;
set autoscale fix;
set xtics 64;
set ytics 1;
#set yrange [185:250]
W=3000;
d=128+2*W
set xrange [d:d+W]
plot  "/tmp/analog_input.csv" using 0:2 with lines, \
	  "/tmp/status.csv" using 0:1 with lines lw 2 lc rgb "black", \
	  '' 				using 0:2 with lines lw 2 lc rgb "red", \
	  '' 				using 0:3 with lines lw 2 lc rgb "red", \
	  '' 				using 0:4 with lines lw 2 lc rgb "black"
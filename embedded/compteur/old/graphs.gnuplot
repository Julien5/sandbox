set terminal pngcairo size 1800,600 enhanced font 'Courier,10'
set output '/tmp/graphs.png'
set multiplot layout 2,1
# plot "data.csv" with lines linestyle 1

set lmargin at screen 0.0
set rmargin at screen 1
set key off
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5
set style line 2 lc rgb 'red' pt 7 ps 0.2 lt 1 lw 2
#set pointintervalbox 3
set grid;
set autoscale fix;
#set xtics 64;
set ytics 1;
set yrange [140:180]
W=5000;
d=128;
set xrange [d:d+W]
plot  "/tmp/analog_input.csv" using 0:2 with lines, \
	  "/tmp/status.csv" using 0:1 with lines lw 2 lc rgb "black", \
	  '' 				using 0:2 with lines lw 2 lc rgb "red", \
	  '' 				using 0:3 with lines lw 2 lc rgb "red", \
	  '' 				using 0:4 with lines lw 2 lc rgb "black"

unset key
set tic scale 0

# Color runs from white to green
#set palette rgbformula -7,2,-7
set cbrange [0:0.1]
unset cbtics
unset yrange
unset colorbox

set view map
splot '/tmp/histogram.dat' matrix with image
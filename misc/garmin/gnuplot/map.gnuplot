set terminal pngcairo size 800,800 enhanced font "sans,18"
set output "/tmp/profile/map.png"

set datafile separator '\t'

unset key
unset ytics
unset xtics
unset key
unset border

set yzeroaxis
set xzeroaxis

set size ratio -1
set xrange [{xmin}:{xmax}]
set yrange [{ymin}:{ymax}];

plot '/tmp/profile/map-track.csv' using 1:2 with lines ls 10 lw 3 notitle, \
	 '/tmp/profile/map-wpt.csv' using 1:2:5 with labels point pt 7 ps 1 \
	 							offset character 1,character 0 tc rgb "black" 
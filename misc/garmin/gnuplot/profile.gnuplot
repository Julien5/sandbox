set terminal pngcairo size 1600,400 enhanced font "sans,18"
set output "/tmp/profile/profile.png"


set key noautotitle
set style line 10 lc rgb 'black' lt 1 lw 2
set style line 12 lc rgb 'black' lt 1 lw 2
set style line 13 lc rgb 'black' lt 3 lw 1 dashtype '.-_'
set style line 14 lc rgb 'black' lt 3 lw 1
#set grid ls 10 # enable grid with specific linestyle
set grid xtics ytics mxtics mytics ls 14, ls 13
set xtics 20
set ytics 200
set mxtics 2
set mytics 2
show grid

set datafile separator '\t'
set xrange [{xmin}:{xmax}]
set yrange [{ymin}:{ymax}];

plot '/tmp/profile/elevation.csv' using 1:2 with lines ls 10 lw 3 notitle, \
	 '/tmp/profile/elevation-wpt.csv' using 1:2:4 with labels point pt 7 ps 1 \
	 					 offset character 0,character 1 tc rgb "black", \
	 '/tmp/profile/elevation-wpt.csv' using 1:2:3 with labels point pt 7 ps 1 \
	 					 offset character 0,character -1 tc rgb "black"  
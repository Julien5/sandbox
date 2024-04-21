set terminal pngcairo size 1600,600 enhanced font "sans,16"
set output "profile.png"


set key noautotitle
set style line 10 lc rgb 'black' lt 1 lw 2
set style line 12 lc rgb 'black' lt 1 lw 2
set style line 13 lc rgb 'black' lt 3 lw 1 dashtype '.-_'
#set grid ls 10 # enable grid with specific linestyle
set grid xtics ytics mxtics mytics ls 13, ls 13
set xtics 20
set ytics 200
set mxtics 2
set mytics 2
show grid

set datafile separator '\t'
set xrange [0:100]
set yrange [0:1500];

set label "A2" center at 40,1200
set label "1010" center at 40,1100 
set arrow 1 from first 40,1075 to 40,1025  lt 1 lw 2 front size 1, 20

plot 'elevation.csv' using 1:2 with lines ls 10 notitle
set terminal pngcairo size 1800,400 enhanced font 'Courier,10'
set output 'replay.png'

set style line 1 lc rgb '#0060ad' lt 1 lw 1 pt 8 pi -1 ps 1.5
set pointintervalbox 3
set grid;
# set autoscale fix;
# set xtics 10

#set yrange [70:100]
#set xrange [0:1500]
#plot "value.0" with lines ls 1
plot '/tmp/replay/values.mean' with lines ls 1
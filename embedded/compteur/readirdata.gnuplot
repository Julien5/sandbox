set terminal pngcairo size 1800,400 enhanced font 'Courier,10'
set output 'ir.png'

# plot "data.csv" with lines linestyle 1

set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5
set style line 2 lc rgb 'red' pt 7 ps 0.2 lt 1 lw 2
set pointintervalbox 3
set grid;
set autoscale fix;
set xtics 500

#set yrange [30:170]
# set xrange [1900:2400]
# set xrange [14000:14320]
set xrange [1100:1700]
plot "data.csv" using 1:2 with points ls 2, \
     "data.csv" using 1:3 with lines ls 1
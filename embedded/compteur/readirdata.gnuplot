set terminal pngcairo size 800,400 enhanced font 'Courier,10'
set output 'ir.png'

# plot "data.csv" with lines linestyle 1

set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5
set pointintervalbox 3
set grid;
plot "data.csv" with lines ls 1 
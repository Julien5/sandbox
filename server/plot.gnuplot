set terminal pngcairo size 600,400 enhanced font 'Verdana,9'
set output 'out.png'
set datafile separator comma

set boxwidth 0.5
set style fill solid
set xrange [0:24*60]

unset key

plot "in.csv" with boxes

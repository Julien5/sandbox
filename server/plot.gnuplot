set terminal pngcairo size 800,400 enhanced font 'Verdana,9'
set output 'millis/2019-01-26--23-21-03.png'
set datafile separator comma

set boxwidth 0.05
set style fill solid
set yrange [0:4000]
set grid
set title "2019-01-26--23-21-03"
unset key

plot "millis/2019-01-26--23-21-03.csv"  using 2 with boxes

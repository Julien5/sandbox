set terminal pngcairo size 800,400 enhanced font 'Verdana,12'
set output 'millis/2019-01-31--07-49-36.png'
set datafile separator comma

set boxwidth 0.05
set style fill solid
set yrange [0:4000]
set grid
set title "2019-01-31--07-49-36"
unset key

plot "millis/2019-01-31--07-49-36.csv"  using 2 with boxes

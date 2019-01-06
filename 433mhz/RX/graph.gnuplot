set terminal pngcairo size 800,500 enhanced font 'Verdana,10'
set output 'data.png'

set datafile separator ";"

set yrange [0:1024]

plot 'data.csv' using 1 with line ls 2


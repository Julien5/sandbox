set terminal pngcairo size 800,800 
set output 'track.png'


unset border
set xtics format " "
set ytics format " "
unset key
set grid

set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5

plot 'track-01.11-11.27.42.dat' with lines linestyle 1
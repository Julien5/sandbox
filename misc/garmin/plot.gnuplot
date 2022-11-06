set terminal pngcairo size 800,800 
set output 'track.png'

unset border
#set xtics format "%f"
#set ytics format "%f"
set xtics format " "
set ytics format " "
unset key
set grid
set size ratio -1

set style rect back fs empty border lc rgb '#008800'
# 555243.958407 5317261.255204
# set object 1 rect from 555243,5317261 to 555343,5317361 lw 1
load "/tmp/boxes-0.gnuplot"

set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1
	
#plot '/tmp/track-1.dat' with linespoints linestyle 1
plot '/tmp/track-1.dat' with line linestyle 1, \
	 '/tmp/track-2.dat' with line linestyle 2
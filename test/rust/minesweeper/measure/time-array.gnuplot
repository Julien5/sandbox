set terminal pngcairo size 800,400 
set output '/tmp/time-array-baseline.png'
set datafile separator "|"

set xlabel "array size parameter L (N=1024L)"
set ylabel "time [seconds]"
# set logscale y 2
set xrange [0:16]
set grid

plot '/tmp/measure.dat' using 1:($3) with linespoints linestyle 3 title "rust", \
	 '/tmp/measure.dat' using 1:($11) with linespoints linestyle 6 title "C++"

set output '/tmp/time-array-margin.png'
plot '/tmp/measure.dat' using 1:($5) with linespoints linestyle 7 title "rust (margin)", \
	 '/tmp/measure.dat' using 1:($7) with linespoints linestyle 9 title "C++ (margin)"

set output '/tmp/time-array-compare.png'
plot '/tmp/measure.dat' using 1:($3) with linespoints linestyle 3 title "rust (baseline)", \
	 '/tmp/measure.dat' using 1:($11) with linespoints linestyle 6 title "C++ (baseline)", \
	 '/tmp/measure.dat' using 1:($5) with linespoints linestyle 7 title "rust (margin)", \
	 '/tmp/measure.dat' using 1:($7) with linespoints linestyle 9 title "C++ (margin)"

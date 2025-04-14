set terminal pngcairo size 800,400 
set output '/tmp/time-array.png'
set datafile separator "|"

set xlabel "array size"
set ylabel "time ratio"

set xrange [0:22]

plot '/tmp/measure.dat' using 1:($3) with linespoints linestyle 1 title "rs-baseline", \
	 '/tmp/measure.dat' using 1:($5) with linespoints linestyle 3 title "rs-parallel (K=1)", \
	 '/tmp/measure.dat' using 1:($7) with linespoints linestyle 5 title "C++"

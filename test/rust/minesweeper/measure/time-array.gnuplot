set terminal pngcairo size 800,400 
set output '/tmp/time-array.png'
set datafile separator "|"

set xlabel "array size parameter L"
set ylabel "time (log scale)"
set logscale y 2
set xrange [0:22]

plot '/tmp/measure.dat' using 1:($5) with linespoints linestyle 3 title "rust (K=1)", \
	 '/tmp/measure.dat' using 1:($7) with linespoints linestyle 5 title "C++"

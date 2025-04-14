set terminal pngcairo size 800,400 
set output '/tmp/time-array-ratio.png'
set datafile separator "|"

set xlabel "array size parameter L"
set ylabel "time ratio"
#set yrange [0:1.2]
set xrange [0:22]
set logscale y 2

plot '/tmp/measure.dat' using 1:($5/$3) with linespoints linestyle 3 title "rust (K=1)", \
	 '/tmp/measure.dat' using 1:($7/$3) with linespoints linestyle 5 title "C++"

set terminal pngcairo size 800,800 
set output '/tmp/time-array.png'
set datafile separator "|"

set xlabel "array size"
set ylabel "time ratio"

plot '/tmp/time.dat' using 1:($3/$3) with linespoints linestyle 1 title "rs-baseline", \
	 '/tmp/time.dat' using 1:($5/$3) with linespoints linestyle 3 title "rs-candidate", \
	 '/tmp/time.dat' using 1:($7/$3) with linespoints linestyle 5 title "C-beautiful", \
	 '/tmp/time.dat' using 1:($9/$3) with linespoints linestyle 7 title "C-fast"
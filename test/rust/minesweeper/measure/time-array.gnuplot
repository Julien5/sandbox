set terminal pngcairo size 800,800 
set output '/tmp/time-array.png'
set datafile separator "|"

set xlabel "array size"
set ylabel "time ratio"
# set yrange [0:6]

plot '/tmp/measure.dat' using 1:($3) with linespoints linestyle 1 title "rs-baseline", \
	 '/tmp/measure.dat' using 1:($5) with linespoints linestyle 3 title "rs-margin", \
	 '/tmp/measure.dat' using 1:($7) with linespoints linestyle 5 title "C-margin", \
	 '/tmp/measure.dat' using 1:($9) with linespoints linestyle 7 title "C-printable"
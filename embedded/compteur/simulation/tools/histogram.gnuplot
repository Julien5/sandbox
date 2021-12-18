set terminal pngcairo size 800,800 enhanced 
set output 'datafile.png'
set view map scale 1
set style data lines
set xrange [ -0.500000 : 4.50000 ] noreverse nowriteback
set yrange [ -0.500000 : 4.50000 ] noreverse nowriteback
set palette rgbformulae -7, 2, -7
splot 'datafile.dat' using 1:2:3 with image
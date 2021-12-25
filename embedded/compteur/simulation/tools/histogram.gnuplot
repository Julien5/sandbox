set terminal pngcairo size 800,800 enhanced 
set output 'datafile.png'
set view map # scale 1
set style data lines
#set palette rgbformulae 30,-31,32
#set pm3d interpolate 2,2
set zrange [0:0.3]
splot 'histogram.dat' using 1:2:3 with pm3d
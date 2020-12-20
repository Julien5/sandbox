set terminal pngcairo size 1800,600 enhanced font 'Courier,10'
set output 'histogram.png'

set lmargin at screen 0.0
set rmargin at screen 1

unset key
set tic scale 0

# Color runs from white to green
#set palette rgbformula -7,2,-7
set cbrange [0:0.1]
unset cbtics
unset yrange
unset colorbox

set view map
splot '/tmp/histogram.dat' matrix with image

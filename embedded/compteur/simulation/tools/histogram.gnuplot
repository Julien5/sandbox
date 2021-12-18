set terminal pngcairo size 800,800 enhanced font 'Courier,10'
set output 'datafile.png'
unset key
set view map scale 1
set style data lines
unset cbtics
set rtics axis in scale 0,0 nomirror norotate  autojustify
set xrange [ -0.500000 : 4.50000 ] noreverse nowriteback
#set x2range [ * : * ] noreverse writeback
set yrange [ -0.500000 : 4.50000 ] noreverse nowriteback
set y2range [ * : * ] noreverse writeback
set zrange [ * : * ] noreverse writeback
set cblabel "Score" 
set cbrange [ 0.00000 : 5.00000 ] noreverse nowriteback
set rrange [ * : * ] noreverse writeback
set palette rgbformulae -7, 2, -7
NO_ANIMATION = 1
#splot 'datafile.dat' nonuniform matrix using 1:2:3 with image
set pm3d interpolate 0,0
splot 'datafile.dat' using 1:2:3 with image
set terminal pngcairo size 800,800 
set output 'time.png'

transform_time(x)=x

plot '/tmp/time.dat' using 1:(transform_time($2)) with line linestyle 1 title "1", \
	 '/tmp/time.dat' using 1:(transform_time($3)) with line linestyle 2 title "2"

set terminal pngcairo size 800,800 
set output 'performance.png'

transform_time(x)=x*30
transform_space(x)=x/5000

plot 'res.dat' using 1:(transform_time($2)) with line linestyle 1 title "time", \
	 'res.dat' using 1:(transform_space($3)) with line linestyle 2 title "space"

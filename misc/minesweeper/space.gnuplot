set terminal pngcairo size 800,800 
set output 'space.png'

transform_space(x)=x

plot '/tmp/space.dat' using 1:(transform_space($2)) with line linestyle 1 title "1", \
	 '/tmp/space.dat' using 1:(transform_space($3)) with line linestyle 2 title "2"

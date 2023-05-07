- generate histogram file:

run ./compteur.pc and grep histogram data
./compteur.pc | grep gnuplot | cut -f3 -d":" > histogram.dat

run gnuplot
gnuplot simulation/tools/histogram.gnuplot ; feh datafile.png 
python3 simulation/tools/generate.py power=1000 repetitions=10;
python3 simulation/tools/generate.py power=1000 repetitions=10;

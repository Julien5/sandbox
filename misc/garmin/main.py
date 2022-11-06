#!/usr/bin/env python3

import readgpx;
import sys;
import plot;
import boxes;
def main():
	T=readgpx.tracks("testride.gpx");
	#for t in tracks:
	#	print(t.string());
	#T=readgpx.tracksfromdir("/home/julien/tracks/2022.10.01/GPX/");
	T=readgpx.tracksfromdir("test");
	counter=0;
	assert(len(T)==2);
	T1=T[0];
	T2=T[1];
	B1=boxes.boxes(T1);
	B2=boxes.boxes(T2);
	I=boxes.Boxes.intersection(B1,B2);
	print(len(I.boxes()));
	plot.plot_track(T1,"/tmp/track-1.dat");
	plot.plot_track(T2,"/tmp/track-2.dat");
	plot.plot_boxes(I,"/tmp/boxes-"+str(counter)+".gnuplot");
		
	
if __name__ == '__main__':
	sys.exit(main())  

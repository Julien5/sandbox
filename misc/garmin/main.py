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
	T=readgpx.tracksfromdir("/home/julien/tracks");
	T=T[0:15];
	S=list();
	print("#tracks:",len(T));
	print("#couples:",len(T)*(len(T)-1)/2);
	for k in range(len(T)):
			for l in range(k):
				T1=T[k];
				T2=T[l];
				print("analyse couple:",k,l);
				B1=boxes.boxes(T1);
				B2=boxes.boxes(T2);
				I=boxes.Boxes.intersection(B1,B2);
				Sloc=I.segments();
				for sloc in Sloc:
					S.append(sloc);	
	plot.plot_track(T1,"/tmp/track-1.dat");
	plot.plot_track(T2,"/tmp/track-2.dat");
	B=boxes.Boxes();
	for s in S:
		B.union(s);	
	plot.plot_boxes(B,"/tmp/boxes-0.gnuplot");
		
	
if __name__ == '__main__':
	sys.exit(main())  

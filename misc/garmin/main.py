#!/usr/bin/env python3

import readgpx;
import sys;
import plot;
import boxes;
import segment

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);	

def main():
	T=readgpx.tracks("testride.gpx");
	#for t in tracks:
	#	print(t.string());
	#T=readgpx.tracksfromdir("/home/julien/tracks/2022.10.01/GPX/");
	T=readgpx.tracksfromdir("/home/julien/tracks");
	#T=T[0:20];
	#T=readgpx.tracksfromdir("test");
	S=list();
	print("#tracks:",len(T));
	B=list();
	for k in range(len(T)):
		B.append(boxes.boxes(T[k]));

	assert(len(B)==len(T));	
	print("#boxes:",len(T));
	pool=list();
	for k in range(len(B)):
		for l in range(k):
			I=boxes.Boxes.intersection(B[k],B[l]);
			Sloc=I.segments();
			# print("couple:",k,l,"=>",len(Sloc),"segments");
			for sloc in Sloc:
				segindexes=segment.similars(pool,sloc);
				for ks in segindexes:
					pool[ks].merge(sloc);
				if not segindexes:
					pool.append(sloc);	
	print("#segments:",len(pool));
	for s in pool:
			print("#segments: surface:",len(s.boxes())," tracks:",len(s.filenames));	
	return;
		
	plot.plot_boxes(B,"/tmp/boxes-0.gnuplot");
	plot.plot_track(T[0],"/tmp/track-1.dat");
	plot.plot_track(T[1],"/tmp/track-2.dat");	
	
if __name__ == '__main__':
	sys.exit(main())  

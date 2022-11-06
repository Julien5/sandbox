#!/usr/bin/env python3

import readgpx;
import sys;
import plot;
import boxes;

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);	

def main():
	T=readgpx.tracks("testride.gpx");
	#for t in tracks:
	#	print(t.string());
	#T=readgpx.tracksfromdir("/home/julien/tracks/2022.10.01/GPX/");
	T=readgpx.tracksfromdir("/home/julien/tracks");
	#T=T[0:40];
	#T=readgpx.tracksfromdir("test");
	S=list();
	print("#tracks:",len(T));
	B=list();
	for k in range(len(T)):
		B.append(boxes.boxes(T[k]));

	assert(len(B)==len(T));	
	print("#boxes:",len(T));		
	for k in range(len(B)):
		for l in range(k):
			I=boxes.Boxes.intersection(B[k],B[l]);
			Sloc=I.segments();
			# print("couple:",k,l,"=>",len(Sloc),"segments");
			for sloc in Sloc:
				S.append(sloc);
	print("#segments:",len(S));
	print("filtering out similar segments")
	F=list();
	for s in S:
		found=False;
		for f in F:
			d=s.distance(f);
			if d<0.05:
				found=True;
				break;
		if not found:
			F.append(s);
	print("#segments:",len(F));			
	B=boxes.Boxes();
	for s in S:
		B.union(s);
		
	plot.plot_boxes(B,"/tmp/boxes-0.gnuplot");
	#plot.plot_track(T1,"/tmp/track-1.dat");
	#plot.plot_track(T2,"/tmp/track-2.dat");	
	
if __name__ == '__main__':
	sys.exit(main())  

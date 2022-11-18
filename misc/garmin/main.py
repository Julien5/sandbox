#!/usr/bin/env python3

import readgpx;
import sys;
import plot;
import boxes;
import segment

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);	

def main():
	#T=readgpx.tracks("testride.gpx");
	#for t in tracks:
	#	print(t.string());
	#T=readgpx.tracksfromdir("/home/julien/tracks/2022.10.01/GPX/");
	T=readgpx.tracksfromdir("/home/julien/tracks");
	T=readgpx.clean(T);
	#T=T[0:20];
	#T=readgpx.tracksfromdir("test");
	#T=readgpx.clean(T);
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

	if True:
		for k in range(len(pool)):
			plot.plot_segment(pool[k],"/tmp/pool-{}.gnuplot".format(k));
		return;	
	#for s in pool:
	s=pool[1];
	print("#segments: surface:",len(s.boxes())," tracks:",len(s.tracks)," length:",s.length()/1000,"km");	
		
	for k in [1]: #range(len(pool)):
		plot.plot_segment(pool[k],"/tmp/pool-{}.gnuplot".format(k));
	
if __name__ == '__main__':
	sys.exit(main())  

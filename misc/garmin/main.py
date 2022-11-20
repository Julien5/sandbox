#!/usr/bin/env python3

import readgpx;
import sys;
import plot;
import boxes;
import segment
import potatoes;

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);	

def main():
	#T=readgpx.tracks("testride.gpx");
	#for t in tracks:
	#	print(t.string());
	#T=readgpx.tracksfromdir("/home/julien/tracks/2022.10.01/GPX/");
	test=False;
	#test=True;	
	if not test:
		T=readgpx.tracksfromdir("/home/julien/tracks");
		T=T[0:20];
	else:	
		T=readgpx.tracksfromdir("test");	
	T=readgpx.clean(T);
	S=list();
	print("#tracks:",len(T));
	B=list();
	for k in range(len(T)):
		B.append(boxes.boxes(T[k]));
	pool0=list();
	for k in range(len(B)):
		for l in range(k):
			I=boxes.Boxes.intersection(B[k],B[l]);
			Sloc=I.segments();
			for s in Sloc:
				pool0.append(s);
				assert(s._bbox);
	Combinations=potatoes.harvest(pool0);
	pool=list();
	for comb in Combinations:
		assert(len(comb)>0);
		I=pool0[comb[0]];
		for k in range(1,len(comb)):
			I=segment.intersection(I,pool0[comb[k]]);
			# => resegmentation !?
		pool.append(I);

	for k in [68,69,70,83,84,85]:
		print(pool[k].trackmarkers());
	markers=potatoes.harvestmarkers(pool);
	print("#markers",len(markers));
	if True:
		for k in range(len(pool)):
			s=pool[k];
			print("#segment-{}: surface:".format(k),len(s.boxes())," tracks:",len(s.tracks)," length:",s.length()/1000,"km");		
			plot.plot_segment(s,"/tmp/segment-{}.gnuplot".format(k));
		print("#segments:",len(pool));	
		return;	
	#for s in pool:
	s=pool[1];
	print("#segments: surface:",len(s.boxes())," tracks:",len(s.tracks)," length:",s.length()/1000,"km");	
		
	for k in [1]: #range(len(pool)):
		plot.plot_segment(pool[k],"/tmp/pool-{}.gnuplot".format(k));
	
if __name__ == '__main__':
	sys.exit(main())  

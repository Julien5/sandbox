#!/usr/bin/env python3

import sys
import cells
import segmentization;
import plot;
import bbox;

def bigcell(Cells,g):
	A=cells.union([Cells[k] for k in g]);
	S=segmentization.segments(A.area());
	if len(S)!=1:
		assert(len(S)==2);
		# what !?
		print("what? there should be one segment but there are",len(S));
		acc.print();
		bb=bbox.cell(A);
		plot.plot_boxes_and_tracks(A.area(),[T[k] for k in color],bb,f"/tmp/S.gnuplot");
		plot.plot_boxes_and_tracks(S[0],[T[k] for k in color],bb,f"/tmp/s-0.gnuplot");
		plot.plot_boxes_and_tracks(S[1],[T[k] for k in color],bb,f"/tmp/s-1.gnuplot");
		plot_trackarea(Cells,T,index,bb);
		assert(0);
	return A;

def statistics(T,area,color):
	assert(len(color)>1);
	parts=dict();
	subtracks=list();
	for c in color:
		parts=segmentization.parts(area,T[c].geometry());
		for part in parts:
			(first,last)=part;
			subtrack=T[c].subtrack(first,last);
			subtracks.append(T[c]);
			subtrack.stats();
	return subtracks;


def output(Cells,T,result,index):
	print("sorting result");
	S=dict();
	for color in result:
		w=len(color);
		if w == 1:
			assert(index in color);
			continue;
		for g in result[color]:
			A=bigcell(Cells,g);
			assert(A.color()==set(color));
			a=len(A.area());
			if not a in S:
				S[a]=set();
			S[a].add(A);
	# we keep only two results.
	sortedareas=sorted(S,reverse=True);
	counter=0;
	for a in sortedareas[0:3]:
		for A in S[a]:
			print(f"{str(A.color()):50s} weigth:{len(A.color()):3d} area:{a:4d}");
			subtracks=statistics(T,A.area(),A.color());
			cat=set([t.category() for t in subtracks]);
			catname=".".join(sorted(cat));
			title=f"segment-{counter:d}";
			bb=bbox.cell(A);
			plot.plot_boxes_and_tracks(A.area(),T,bb,f"/tmp/U-{catname:s}-{title:s}.gnuplot");
			counter=counter+1;
	
def main():
	pass;
	
if __name__ == '__main__':
	sys.exit(main())  

#!/usr/bin/env python3

import sys
from . import cells
from . import segmentization;
from . import plot;
from . import bbox;
import datetime;
import math;
import readgpx;
import output;
from . import select_segments;

def refsubstrack(track,area):
	parts=segmentization.parts(area,track.geometry());
	D=dict();
	for (first,last) in parts:
		sub=track.subtrack(first,last)
		dist=sub.distance();
		D[dist]=sub;
	dmax=max(D);
	return D[dmax];

def have_same_distance(subtrack_ref,subtrack):
	D0=subtrack_ref.distance();
	D=subtrack.distance();
	r=abs(D0-D)/D0;
	#print(D0,D,r);
	return r<0.15;

def statistics(T,area,color,index):
	assert(len(color)>1);
	parts=dict();
	subtracks=list();
	ref=refsubstrack(T[index],area)
	for c in color:
		#if not "090" in T[c].name():
		#	continue;
		parts=segmentization.parts(area,T[c].geometry());
		assert(parts);
		for part in parts:
			(first,last)=part;
			subtrack=T[c].subtrack(first,last);
			if not have_same_distance(ref,subtrack):
				continue;
			subtracks.append(T[c]);
			output.print_stats(subtrack);
			t=subtrack;
			filename=f"/tmp/{t.category():s}-{t.name():s}.gpx";
			#print("write",c,"as",filename);
			readgpx.write(t,filename);
	return subtracks;

def process(Cells,T,segments_dictionary,index):
	threshold = 100;
	if T[index].category()=="cycling":
		threshold = 400;
	result2=select_segments.interesting_areas(Cells,segments_dictionary,threshold);
	counter=0;
	for color in result2:
		for g in result2[color]:
			bigCell=cells.Segment(g).bigCell(Cells);
			bigArea=bigCell.area();
			if len(color)==1:
				continue;
			subtracks=statistics(T,bigArea,color,index);	
			category=".".join(sorted(set([t.category() for t in subtracks])));
			title=f"segment-{counter:d}";
			filename=f"/tmp/{category:s}-{title:s}.gnuplot";
			print(f"{filename:28s} #visits:{len(color):d}  #area:{len(bigArea):d}");
			print("-"*60)
			bb=bbox.cell(bigCell);
			plot.plot_boxes_and_tracks(bigArea,[T[k] for k in color],bb,filename);
			counter=counter+1;
	
def main():
	pass;
	
if __name__ == '__main__':
	sys.exit(main())  

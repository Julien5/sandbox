#!/usr/bin/env python3

import sys
import cells
import segmentization;
import plot;
import bbox;
import datetime;
import math;
import readgpx;

def bigcell(Cells,g):
	A=cells.union([Cells[k] for k in g]);
	S=segmentization.subgroups(A.area());
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

def stats(track):
	print(f"{track.name():10s} ",end=" | ");
	if not track.points():
		print("empty");
		return;
	if track.distance()>500000:
		print(track.string())	
		assert(False);	
	t0=(sorted(track.points())[0]+datetime.timedelta(hours=2)).strftime("%H:%M");
	print(f"{t0:5s}",end=" | ");
	#print(f"#{len(track.points()):5d}",end=" | ");
	print(f"{track.distance()/1000:5.1f} km",end=" | ");
	ds=track.duration().total_seconds();
	hours=math.floor(ds/3600);
	seconds=ds-3600*hours;
	minutes=math.floor(seconds/60);
	print(f"{hours:02d}:{minutes:02d}",end=" | ");
	speed=0;
	if track.speed():
		speed=3600*track.speed()/1000;
	print(f"{speed:4.1f} kmh",end=" |");
	print("");

def refsubstrack(track,area):
	parts=segmentization.parts(area,track.geometry());
	D=dict();
	for (first,last) in parts:
		sub=track.subtrack(first,last)
		dist=sub.distance();
		D[dist]=sub;
	dmax=max(D);
	return D[dmax];

def isgood(subtrack_ref,subtrack):
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
			if not isgood(ref,subtrack):
				continue;
			subtracks.append(T[c]);
			stats(subtrack);
			t=subtrack;
			filename=f"/tmp/{t.category():s}-{t.name():s}.gpx";
			#print("write",c,"as",filename);
			readgpx.write(t,filename);
	return subtracks;

def similarity(Cells,I1,I2):
	a1=[Cells[i] for i in I1.indices()];
	a2=[Cells[i] for i in I2.indices()];
	s1=cells.union(a1).area();
	s2=cells.union(a2).area();
	if len(s1) < 100 or len(s2) < 100:
		# should have been removed by filter_list.
		assert(0);
		return -1;
	diff=s1.symmetric_difference(s2);
	u=s1.union(s2);
	return 1-(len(diff)/len(u));

def similar(Cells,I1,I2):
	return similarity(Cells,I1,I2)>0.7;

def similar_groups(Cells,A):
	ret=dict();
	R=range(len(A))
	for k in R:
		ret[k]=set();
		# TODO: do not compute A[k]~A[l] and A[l]~A[k]
		for l in R:
			if similar(Cells,A[k],A[l]):
				ret[k].add(l);
	return ret;			
	
def crosssimilarities(Cells,A):
	return similar_groups(Cells,A);			

def total_area(Cells,segment):
	return len(segment.bigCell(Cells).area());

def filter_list(Cells,segments,threshold):
	ret=list();
	for seg in segments:
		if total_area(Cells,seg)>threshold:
			ret.append(seg);
	return ret;

def area(Cells,indices):
	return len(cells.Segment(indices).bigCell(Cells).area());

def interesting_areas(Cells,segments_dictionary):
	debug=False;
	# remove small areas
	all_segments=list();
	for color in segments_dictionary.colors():
		segs=filter_list(Cells,segments_dictionary.segments(color),100);
		for seg in segs:
			all_segments.append(seg);
	if debug:		
		print("#segments:",len(all_segments));		

	D=crosssimilarities(Cells,all_segments);
	# D is a dict
	# D[k] contains the l s.t. seg[k] and seg[l] are similar
	# (seg[i] stands for all_segments[i]).
	# "we gather them together" (make equivalence classes)
	U=set();
	for k in D:
		U.add(tuple(D[k]));

	# U is a set
	# Its elements are the equivalence classes defined per D.
	# We need to aggregate them because the union of two classes
	# might result in the same domain, like
	#     {1,2} U {3,4} = {1,3} U {2,4}
	aggregated=dict();	
	for C in U:
		# C = {1,2} (for example)
		# => we need to group them like union, or intersection, or something
		# and make ONE element.
		Domains={all_segments[k].indices() for k in C};
		U_domain=set().union(*Domains);
		domain=tuple(U_domain);
		if not domain in aggregated:
			aggregated[domain]=set();

		Colors={segments_dictionary.color(all_segments[k]) for k in C};
		aggregated[domain]=aggregated[domain].union(*Colors);
			

	# repack so that the output type equals the input type
	ret=dict();	
	for domain in aggregated:
		color=tuple(aggregated[domain]);
		if debug:
			print(color,domain);
		assert(len(color)>=1);
		if len(color) == 1:
			if debug:
				print("skip:",color,domain);
			continue;
		ret[color]={domain};
	if debug:
		count=0;
		for color in ret:
			indices=list(ret[color])[0];
			segment=cells.Segment(indices);
			print(f"#{count:d}",len(color),"->",len(segment.bigCell(Cells).area()));
			#print(f"#{count:d}",color,"->",ret[color]);
			#print("");
			count=count+1;
	return ret;		

def output(Cells,T,segments_dictionary,index):
	result2=interesting_areas(Cells,segments_dictionary);
	counter=0;
	for color in result2:
		for g in result2[color]:
			area=bigcell(Cells,g).area();
			if len(color)==1:
				continue;
			subtracks=statistics(T,area,color,index);	
			category=".".join(sorted(set([t.category() for t in subtracks])));
			title=f"segment-{counter:d}";
			filename=f"/tmp/{category:s}-{title:s}.gnuplot";
			print(f"{filename:28s} #visits:{len(color):d}  #area:{len(area):d}");
			print("-"*60)
			bb=bbox.cell(bigcell(Cells,g));
			plot.plot_boxes_and_tracks(area,[T[k] for k in color],bb,filename);
			counter=counter+1;
	
def main():
	pass;
	
if __name__ == '__main__':
	sys.exit(main())  

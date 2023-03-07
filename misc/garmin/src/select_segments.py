#!/usr/bin/env python3

import cells;
import segmentization;

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

def interesting_areas(Cells,segments_dictionary,threshold):
	debug=False;
	# remove small areas
	all_segments=list();
	for color in segments_dictionary.colors():
		segs=filter_list(Cells,segments_dictionary.segments(color),threshold);
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



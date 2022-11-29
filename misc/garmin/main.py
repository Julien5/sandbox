#!/usr/bin/env python3

import readgpx;
import sys;
import plot;
import boxes;
import cells;
import potatoes;
import coloredmap;
import segmentization;
import bbox;
import segment;
import neighboor;
import datetime;

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);

def plot_trackarea(Cells,T,n):
	areas=[c.area() for c in Cells if n in c.color()];
	colors=[c.color() for c in Cells if n in c.color()];
	tracks=T;
	bb=bbox.cells([c for c in Cells if n in c.color()]);
	plot.plot_areas(areas,colors,tracks,bb,"/tmp/trackarea-{}.gnuplot".format(n));

def getfriends(Cells,T,color):
	friends=set();
	for k in range(len(Cells)):
		if color in Cells[k].color() and len(Cells[k].color())>1:
			friends.update(Cells[k].color());
	
	if not friends:
		return set();	

	TrackCell=cells.union([c for c in Cells if color in c.color()]);
	groups=set();
	for friend in friends:
		if friend == color:
			continue;	
		c=cells.color([c for c in Cells if {color,friend}.issubset(c.color())]);
		tu=tuple(sorted(c));
		if len(c)<2:
			assert(color in c);
		U=cells.union([c for c in Cells if {color,friend}.issubset(c.color())]);
		#print(f"friend:{friend:d} area:{len(U.area()):3d} distance{TrackCell.distance(U):3.2f}");
		assert(len(U.color())>1);
		if len(U.area())<100:
			continue;
		large=len(U.area())>500;#TrackCell.distance(U)<0.6;
		many=len(c)>=min(len(T),4);
		if large or many:
			groups.add(tu);
	F=set();		
	for g in groups:
		for k in g:	
			F.add(k);
	#if len(F) != len(groups):
		#print(color,"has",len(F),"friends in",len(groups),"groups");
		#for g in groups:
		#	print(g)
	return groups;

def minarea(category):
	if category=="cycling":
		return 1500;
	return 100;

def sortgroups(Cells,T,groups):
	BigCells=list();
	category=T[0].category();
	for group in groups:	
		U=cells.union([c for c in Cells if set(group).issubset(c.color())]);
		limit=500;
		if len(U.area())>minarea(category):
			BigCells.append(U);

	result=list();
	for k in range(len(BigCells)):
		found=False;
		Bk=BigCells[k];
		for l in range(len(result)):
			r=result[l];
			d=Bk.distance(r);
			if d<0.1:
				found=True;
				result[l]=cells.uunion([r,Bk])
				break;
		if not found:
			result.append(Bk);	
	return result;	

def display(Cells,T,result):
	counter=0;
	bb=bbox.cells(Cells);
	for BigCell in result:
		S=segmentization.segments(BigCell.area());
		for k in range(len(S)):
			s=S[k];
			title=f"segment-{counter:d}";
			display_segment(T,s,bb,title,BigCell.color());
			counter=counter+1;
		print(f"{str('-'*40):40s}");
			

def display_segment(T,area,bb,title,color):
	tracks=[T[c] for c in sorted(color)];
	cat=set([t.category() for t in tracks]);
	catname="_".join(sorted(cat));
	subtracks=statistics(T,area,color);
	if subtracks:
		plot.plot_boxes_and_tracks(area,subtracks,bb,f"/tmp/U-{catname:s}-{title:s}.gnuplot");

def statistics(T,area,color):
	assert(len(color)>1);
	parts=dict();
	subtracks=list();
	for c in color:
		parts=segmentization.parts(area,T[c].geometry());
		for part in parts:
			(first,last)=part;
			subtrack=T[c].subtrack(first,last);
			if subtrack.distance()<1000:
				continue;
			subtracks.append(T[c]);
			subtrack.stats();
	return subtracks;		

def processtracks(T):
	#print("#tracks:",len(T));
	B=dict();
	for k in range(len(T)):
		B[k]=boxes.boxes(T[k]);
	Map=coloredmap.indexes(coloredmap.colors(B));
	Cells=list();
	for color in Map:
		S=segmentization.segments(Map[color]);
		for s in S:
			# ignore cells with one track only.	
			#if len(s) == 1:
			#	continue;
			Cells.append(cells.Cell(s,set(color)));
	#print("#cells",len(Cells)," area:",sum([len(c.area()) for c in Cells]));
	# cleanup is evil
	Cells=cells.cleanup(Cells);
	#print("#cells",len(Cells)," area:",sum([len(c.area()) for c in Cells]));
	groups=set();
	#A=[c.area() for c in Cells];
	#C=[len(c.color()) for c in Cells];
	#plot.plot_areas(A,C,T,bbox.cells(Cells),"/tmp/map.gnuplot");
	for color in range(len(T)):
		groups.update(getfriends(Cells,T,color));
	result=sortgroups(Cells,T,groups);
	display(Cells,T,result);
			
		
def main():
	test=False;
	#test=True;	
	if not test:
		T=readgpx.tracksfromdir("/home/julien/tracks/");
		#T=readgpx.tracksfromdir("/home/julien/tracks/2022.11.25");
		#T=T[0:20];
	else:	
		T=readgpx.tracksfromdir("test");
	T=readgpx.clean(T);
	for t in T:
		readgpx.write(t,f"/tmp/{t.category():s}-{t.name():s}.gpx");
	C=dict();
	for t in T:
		#t.stats();
		if not t.category() in C:
			C[t.category()]=list();
		C[t.category()].append(t);
	for cat in C:
		if cat == "none":# or cat == "cycling":
			continue;
		S=dict();
		for t in C[cat]:
			if not t.distance() in S:	
				S[t.distance()]=set();	
			S[t.distance()].add(t);
		for d in sorted(S):	
			for t in S[d]:
				t.stats();	
		L=sum([t.distance() for t in C[cat]]);
		D=sum([t.duration().total_seconds() for t in C[cat]]);
		print(f"total {cat:10s}: {L/1000:6.1f} km | {D/3600:4.1f}h");
		#processtracks(C[cat]);
		
if __name__ == '__main__':
	sys.exit(main())  

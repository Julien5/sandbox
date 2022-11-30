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
import gather;

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);

def plot_trackarea(Cells,T,n):
	areas=[c.area() for c in Cells if n in c.color()];
	colors=[c.color() for c in Cells if n in c.color()];
	tracks=T;
	bb=bbox.cells([c for c in Cells if n in c.color()]);
	plot.plot_areas(areas,colors,tracks,bb,"/tmp/trackarea-{}.gnuplot".format(n));

def getCellList(Cells,T,index):
	assert(type(index)==type(0));
	friends=set();
	L=list();
	I=neighboor.cells_with_color(Cells,index);
	for p in T[index].geometry():
		cellIndex=neighboor.cell_lookup_index(Cells,I,p);
		assert(cellIndex != None);
		if not L or {cellIndex} != L[-1]:
			L.append({cellIndex});
	return L;		

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
	for BigCell in result:
		S=segmentization.segments(BigCell.area());
		bb=bbox.cells([BigCell]);
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
		print(title);	
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

def processSingleTrack(Cells,T,index):
	L=getCellList(Cells,T,index);
	print("index:", index," cells:",L);
	acc=gather.Accumulator();
	assert(L);
	assert(type(L[0]) == type(set()));
	gather.walk(Cells,L,acc);
	R=acc.result();
	# L = [{1},{2},{3}] (cell indexes)
	# R[color] is a set. Each element is a set of index.
	# example:
	# R[(1)] = {{1,3,4},{6}}
	acc.print();
	assert(acc.check(Cells,L));
	for color in acc.result():
		w=len(color);	
		for g in R[color]:
			a=len(cells.union([Cells[k] for k in g]).area());
			print(f"{str(set(color)):50s} weigth:{w:3d} area:{a:4d}");
			

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
	# Cells=cells.cleanup(Cells);
	#print("#cells",len(Cells)," area:",sum([len(c.area()) for c in Cells]));
	groups=set();
	#A=[c.area() for c in Cells];
	#C=[len(c.color()) for c in Cells];
	#plot.plot_areas(A,C,T,bbox.cells(Cells),"/tmp/map.gnuplot");
	for color in [len(T)-1]:#range(len(T)):
		processSingleTrack(Cells,T,color);	
			
		
def main():
	test=False;
	#test=True;	
	if not test:
		#T=readgpx.tracksfromdir("/home/julien/tracks/");
		T=readgpx.tracksfromdir("/home/julien/tracks/2022.11.25");
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
		processtracks(C[cat]);
		print()
		
if __name__ == '__main__':
	sys.exit(main())  

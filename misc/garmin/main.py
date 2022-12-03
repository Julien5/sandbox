#!/usr/bin/env python3

import readgpx;
import sys;
import plot;
import boxes;
import cells;
import coloredmap;
import segmentization;
import bbox;
import neighboor;
import datetime;
import gather;
import output;

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);

def plot_trackarea(Cells,T,n,bb=None):
	areas=[c.area() for c in Cells if n in c.color()];
	colors=[c.color() for c in Cells if n in c.color()];
	tracks=T;
	if not bb:
		bb=bbox.cells([c for c in Cells if n in c.color()]);
	plot.plot_areas(areas,colors,tracks,bb,"/tmp/trackarea-{}.gnuplot".format(n));

def celllist(Cells,I,hitlist):
	sublist=list();
	for s in hitlist:
		for i in I:
			if s in Cells[i].area():
				if not sublist or i != sublist[-1]:
						sublist.append(i);
	return sublist;					
	
def getCellList(Cells,T,index):
	assert(type(index)==type(0));
	friends=set();
	L=list();
	I=neighboor.cells_with_color(Cells,index);
	assert(I);
	G=T[index].geometry()
	for k in range(len(G)-1):
		u=G[k];
		v=G[k+1];
		sublist=celllist(Cells,I,boxes.hitlist(u,v));
		for cellIndex in sublist:
			assert(cellIndex != None);
			if not L or {cellIndex} != L[-1]:
				L.append({cellIndex});
	assert(L);
	# Because the set I includes surroundings, we have:
	# len(I) <= len(L)
	# but because L go hit the same cell multiple times, we have
	# len(I) => len(L)
	# => we cannot assert anything about there lengths.
	return L;		

def processSingleTrack(Cells,T,index):
	L=getCellList(Cells,T,index);
	# print("index:", index," cells:",L);
	acc=gather.Accumulator();
	assert(L);
	assert(type(L[0]) == type(set()));
	gather.walk(Cells,L,acc);
	R=acc.result();
	# L = [{1},{2},{3}] (cell indexes)
	# R[color] is a set. Each element is a set of index.
	# example:
	# R[(1)] = {{1,3,4},{6}}
	assert(acc.check(Cells,L));
	output.output(Cells,T,acc.result(),index);

def processtracks(T):
	B=dict();
	for k in range(len(T)):
		B[k]=boxes.boxes(T[k]);
	Map=coloredmap.indexes(coloredmap.colors(B));
	Cells=list();
	assert(Map);
	for color in Map:
		S=segmentization.segments(Map[color]);
		for s in S:
			# ignore cells with one track only.	
			#if len(s) == 1:
			#	continue;
			Cells.append(cells.Cell(s,set(color)));
	assert(Cells);		
	#A=[c.area() for c in Cells];
	#C=[len(c.color()) for c in Cells];
	#plot.plot_areas(A,C,T,bbox.cells(Cells),"/tmp/map.gnuplot");
	for color in [len(T)-1]:#range(len(T)):
		processSingleTrack(Cells,T,color);	
			
		
def main():
	test=False;
	#test=True;
	print("read files..");
	if not test:
		dir="/home/julien/tracks/";
		# dir="/home/julien/tracks/2022.11.25";
		if len(sys.argv)>1:
			dir=sys.argv[1];
		T=readgpx.tracksfromdir(dir);
		#T=T[0:20];
	else:	
		T=readgpx.tracksfromdir("test");
	print("clean tracks..");
	T=readgpx.clean(T);
	print("categorizing..");
	for t in T:
		readgpx.write(t,f"/tmp/{t.category():s}-{t.name():s}.gpx");
	C=dict();
	for t in T:
		#t.stats();
		if not t.category() in C:
			C[t.category()]=list();
		C[t.category()].append(t);
	print("OK");	
	for cat in ["cycling","running"]:
		S=dict();
		if not cat in C:
			continue;
		for t in C[cat]:
			if not t.distance() in S:	
				S[t.distance()]=set();	
			S[t.distance()].add(t);
		print("# category",cat);
		for d in sorted(S):	
			for t in S[d]:
				t.stats();	
		L=sum([t.distance() for t in C[cat]]);
		D=sum([t.duration().total_seconds() for t in C[cat]]);
		print(f"total {cat:10s}: {L/1000:6.1f} km | {D/3600:4.1f}h");
		print("-"*55)
		processtracks(C[cat]);
		print()
		
if __name__ == '__main__':
	sys.exit(main())  

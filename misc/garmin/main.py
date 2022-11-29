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
import neighboor;

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);

def plot_trackarea(Cells,T,n):
	areas=[c.area() for c in Cells if n in c.color()];
	colors=[c.color() for c in Cells if n in c.color()];
	tracks=T;
	bb=bbox.cells([c for c in Cells if n in c.color()]);
	plot.plot_areas(areas,colors,tracks,bb,"/tmp/trackarea-{}.gnuplot".format(n));

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
	S=list();
	print("#tracks:",len(T));
	B=dict();
	for k in range(len(T)):
		B[k]=boxes.boxes(T[k]);
	colors=coloredmap.colors(B);
	indexes=coloredmap.indexes(colors);
	Cells=list();
	for color in indexes:
		S=segmentization.segments(indexes[color]);
		for s in S:
			# ignore cells with one track only.	
			#if len(s) == 1:
			#	continue;
			Cells.append(cells.Cell(s,set(color)));
	print("#colors",len(indexes));
	print("#cells",len(Cells)," area:",sum([len(c.area()) for c in Cells]));
	print("cleanup...");
	# cleanup is evil
	Cells=cells.cleanup(Cells);
	print("#cells",len(Cells)," area:",sum([len(c.area()) for c in Cells]));
	#bb=bbox.cells(Cells);
	for k in []:#range(len(Cells)):
		cell=Cells[k];
		print("cell-{}".format(k),f"area:{len(cell.area()):5d}"," #tracks:",set(cell.color()));
		tracks=[T[c] for c in cell.color()];
		plot.plot_boxes_and_tracks(cell.area(),tracks,bbox.cell(cell),"/tmp/cell-{}.gnuplot".format(k));

	#plot_trackarea(Cells,T,10);
	color=len(T)-1; # last tour
	friends=set();
	for k in range(len(Cells)):
		if color in Cells[k].color() and len(Cells[k].color())>1:
			friends.update(Cells[k].color());
	print("friends:",friends)
	
	friendsets=set();		
	for friend in friends:
		c=cells.color([c for c in Cells if {color,friend}.issubset(c.color())]);
		tu=tuple(sorted(c));
		if len(c)<2:
			assert(color in c);
		#print(friend,U.color(),len(U.area()));
		print(friend," -> ",c);
		friendsets.add(tu);
	print(len(friends),"friends");	
	print(len(friendsets),"friendsets");	

	track=T[color];
	cell=dict();
	for k in range(len(track.geometry())):
		point=track.geometry()[k];	
		cell[k]=neighboor.cell_lookup_color(Cells,color,point);
		if k==0:
			continue;
		if cell[k] != cell[k-1] and not cell[k] is None:
			c=Cells[cell[k]];	
			#print(cell[k-1],"->",cell[k]," area:",len(c.area()),c.color());

	result=dict();		
	for friendset in friendsets:	
		U=cells.union([c for c in Cells if set(friendset).issubset(c.color())]);
		key=len(U.area());
		if not key in result:
			result[key]=set();
		result[key].add(friendset);

	bb=bbox.cells([c for c in Cells if color in c.color()]);
	for a in sorted(result):
		for s in result[a]:
			print(f"{str(sorted(set(s))):80s} {a:5d}");
			U=cells.union([c for c in Cells if set(s).issubset(c.color())]);
			tracks=[T[c] for c in sorted(U.color())];
			plot.plot_boxes_and_tracks(U.area(),tracks,bb,f"/tmp/U-{a:05d}.gnuplot");
	
if __name__ == '__main__':
	sys.exit(main())  

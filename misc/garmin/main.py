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

def main():
	#T=readgpx.tracks("testride.gpx");
	#for t in tracks:
	#	print(t.string());
	#T=readgpx.tracksfromdir("/home/julien/tracks/2022.10.01/GPX/");
	test=False;
	#test=True;	
	if not test:
		T=readgpx.tracksfromdir("/home/julien/tracks/2022.11.25");
		#T=T[0:20];
	else:	
		T=readgpx.tracksfromdir("test");	
	T=readgpx.clean(T);
	for k in range(len(T)):
		tour=T[k];
		print("tour-{}".format(k),tour.name());	
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
	print("#cells",len(Cells));
	Cells=cells.cleanup(Cells);
	print("#cells",len(Cells));
	#bb=bbox.cells(Cells);
	for k in [0]:#range(len(Cells)):
		cell=Cells[k];
		print("cell-{}".format(k),f"area:{len(cell.area()):5d}"," #tracks:",set(cell.color()));
		tracks=[T[c] for c in cell.color()];
		#if len(cell.area())>100:
		plot.plot_boxes_and_tracks(cell.area(),tracks,bbox.cell(cell),"/tmp/cell-{}.gnuplot".format(k));

	areas=[c.area() for c in Cells if 10 in c.color()];
	colors=[c.color() for c in Cells if 10 in c.color()];
	tracks=T;
	bb=bbox.cells([c for c in Cells if 10 in c.color()]);
	plot.plot_areas(areas,colors,tracks,bb,"/tmp/areas.gnuplot");
	return;
	print("compute map");
	M=neighboor.neighboorsmap(Cells,T);
	mothers=cells.mothers(Cells,M);
	print("mothers:",len(mothers));
	for m in mothers:
		cell=Cells[m];
		print("mother:",m,f"area:{len(cell.area()):5d}"," #tracks:",set(cell.color()),"#neighboors:",M[m])
		tracks=[T[c] for c in cell.color()];
		plot.plot_boxes_and_tracks(cell.area(),tracks,bbox.cells([cell]),"/tmp/mother-{}.gnuplot".format(m));
		for n in M[m]:
			U=cells.union([cell,Cells[n]]);
			assert(U.color());
			if len(U.color())>=1:
				tracks=[T[c] for c in U.color()];
				plot.plot_boxes_and_tracks(U.area(),tracks,bbox.cells([U]),"/tmp/mother-{}-{}.gnuplot".format(m,n))
	A=cells.ColorAccumulator(Cells);
	print("walking...");
	for m in mothers:
		cells.walk(Cells,m,M,A);
	cont=A.container()
	counter=0
	for u in cont:
		area=cont[u];
		color=set(u);
		U=cells.Cell(area,color);
		if len(area)>100:
			print(counter,":",len(u)," tracks on area",len(cont[u]),":",color);
			tracks=[T[c] for c in U.color()];
			plot.plot_boxes_and_tracks(area,tracks,bbox.cells([U]),"/tmp/good-{}.gnuplot".format(counter))
			counter=counter+1;
	print("goodbye");		

if __name__ == '__main__':
	sys.exit(main())  

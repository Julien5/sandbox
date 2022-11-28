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
		T=readgpx.tracksfromdir("/home/julien/tracks");
		#T=T[0:10];
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
			if len(s) == 1:
				continue;
			Cells.append(cells.Cell(s,color));
	print("#colors",len(indexes));
	print("#cells",len(Cells));
	Cells=cells.cleanup(Cells);
	print("#cells",len(Cells));
	bb=bbox.cells(Cells);
	for k in range(len(Cells)):
		cell=Cells[k];
		print("cell-{}".format(k),f"area:{len(cell.area()):5d}"," #tracks:",set(cell.color()));
		tracks=[T[c] for c in cell.color()];
		plot.plot_boxes_and_tracks(cell.area(),tracks,bb,"/tmp/cell-{}.gnuplot".format(k));

	areas=[c.area() for c in Cells];
	colors=[len(c.color()) for c in Cells];
	tracks=[T[0]];
	plot.plot_areas(areas,colors,tracks,bb,"/tmp/areas.gnuplot");
	return;

	
	print("compute map");
	M=neighboor.neighboorsmap(Cells,T);
	print("#M",len(M));
	mothers=cells.mothers(Cells,M);
	print("mothers:",mothers);
	counter=0;
	for m in mothers:
		cell=Cells[m];
		tracks=[T[c] for c in cell.color()];
		plot.plot_boxes_and_tracks(cell.area(),tracks,bb,"/tmp/mother-{}.gnuplot".format(counter));
		counter=counter+1;
	A=cells.ColorAccumulator(Cells);
	for m in mothers:
		cells.walk(m,M,A);
	print(A.container());	
	return;
	for tracks in Combinations:
		if len(tracks)<20:
			continue;
		area=Combinations[tracks];
		home=(int(555230/50),int(5317240/50));
		area.add(home);
		if len(area)<10:
			continue;	
		B=segment.Segment(area,tracks);
		plot.plot_boxes_and_tracks(B,tracks,"/tmp/comb-{}.gnuplot".format(counter));
		counter=counter+1;
	return;	
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

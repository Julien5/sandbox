#!/usr/bin/env python3

import track
import geometry;
import segment;

def center(point):
	#(n,m)=(11138, 106351);
	(n,m)=(0,0)
	W=50;
	return geometry.Point(point.x()-n*W,point.y()-m*W);

def plot_track(track,filename):
	s=str();
	for q in track.geometry():
		p=center(q);
		s+=f"{p.x():f} {p.y():f}\n";
	f=open(filename,'w');
	f.write(s);
	f.close();
		
def plot_boxes(boxes,filename):
	s=str();
	# set object 1 rect from 555243,5317261 to 555343,5317361 lw 1
	counter=1;
	for p in boxes.boxes():
		A=center(boxes.geomin(p));
		B=center(boxes.geomax(p));
		s+=f"set object {counter:d} rect from {A.x():d},{A.y():d} to {B.x():d},{B.y():d} lw 1\n"
		counter = counter + 1;
	f=open(filename,'w');
	f.write(s);
	f.close();


def plot_boxes_and_tracks(boxes,tracks,filename):
	boxesfilename=filename+".segment";
	plot_boxes(boxes,boxesfilename);
	T=list(tracks);
	plots=list();
	output=filename+".png";
	for k in range(len(T)):
		datfile=filename+".track."+str(k)+".dat"; 
		plot_track(T[k],datfile);
		title=T[k].name()
		plots.append("\'{}\' with line linestyle {} title \"{}\"".format(datfile,k+1,title));
	tracksplot="plot "+", \\\n     ".join(plots);
	tmpl=open("plot.tmpl",'r').read();
	tmpl=tmpl.replace("{output}",output);
	tmpl=tmpl.replace("{boxesfilename}",boxesfilename);
	tmpl=tmpl.replace("{tracksplot}",tracksplot);
	(xmin,xmax,ymin,ymax)=boxes.bbox();
	margin=max([(xmax-xmin),(ymax-ymin)])*10;
	tmpl=tmpl.replace("{xmin}",str(segment.geomin((xmin,ymin)).x()-margin));
	tmpl=tmpl.replace("{ymin}",str(segment.geomin((xmin,ymin)).y()-margin));
	tmpl=tmpl.replace("{xmax}",str(segment.geomin((xmax,ymax)).x()+margin));
	tmpl=tmpl.replace("{ymax}",str(segment.geomin((xmax,ymax)).y()+margin));
	open(filename,'w').write(tmpl);

def plot_segment(segment,filename):
	plot_boxes_and_tracks(segment,segment.tracks,filename);	

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
		
def plot_boxes(boxset,filename,color=1):
	s=str();
	# set object 1 rect from 555243,5317261 to 555343,5317361 lw 1
	counter=1;
	for p in boxset:
		A=center(segment.geomin(p));
		B=center(segment.geomax(p));
		s+=f"set object {counter:d} rect from {A.x():d},{A.y():d} to {B.x():d},{B.y():d} lw {color:d} lc {color:d}\n"
		counter = counter + 1;
	f=open(filename,'w');
	f.write(s);
	f.close();

def make_tracksplot(tracks,filenamebase):
	plots=list();
	T=list(tracks);
	for k in range(len(T)):
		datfile=filenamebase+".track."+str(k)+".dat"; 
		plot_track(T[k],datfile);
		title=T[k].name()
		plots.append("\'{}\' with line linestyle {} title \"{}\"".format(datfile,k+1,title));
	return "plot "+", \\\n     ".join(plots);	

def plot_boxes_and_tracks(boxes,tracks,filename):
	boxesfilename=filename+".segment";
	plot_boxes(boxes.boxes(),boxesfilename);
	plots=list();
	output=filename+".png";
	tracksplot=make_tracksplot(tracks,filename);
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

def plot_segments_intersects(S,filename):
	assert(len(S)==2);

	output=filename+".png";
	tmpl=open("plot.tmpl",'r').read();
	tmpl=tmpl.replace("{output}",output);

	boxes=[];
	S1=S[0].boxes();
	S2=S[1].boxes();
	S1alone = S1;
	S2alone = S2;
	Sdiff = S1.symmetric_difference(S2);
	for s in [S1alone,S2alone,Sdiff]:
		index=len(boxes);
		boxfilename=filename+"box"+str(index)+".segment";
		color=2;
		if index < 2:
			color=1;
		plot_boxes(s,boxfilename,color);
		boxes.append("load \""+boxfilename+"\"");	
	tmpl=tmpl.replace("load \"{boxesfilename}\"","\n".join(boxes));
	tracks=S[0].tracks.union(S[1].tracks);
	tmpl=tmpl.replace("{tracksplot}",make_tracksplot(tracks,filename));
	(xmin,xmax,ymin,ymax)=S[0].bbox();
	margin=max([(xmax-xmin),(ymax-ymin)])*10;
	tmpl=tmpl.replace("{xmin}",str(segment.geomin((xmin,ymin)).x()-margin));
	tmpl=tmpl.replace("{ymin}",str(segment.geomin((xmin,ymin)).y()-margin));
	tmpl=tmpl.replace("{xmax}",str(segment.geomin((xmax,ymax)).x()+margin));
	tmpl=tmpl.replace("{ymax}",str(segment.geomin((xmax,ymax)).y()+margin));
	open(filename,'w').write(tmpl);
	

def plot_segment(segment,filename):
	plot_boxes_and_tracks(segment,segment.tracks,filename);	

#!/usr/bin/env python3

import track
import geometry;
from . import boxes;

def boxwidth():
	return boxes.boxwidth();	

def geomin(p):
	(n,m)=p;
	W=boxwidth();
	return geometry.Point(n*W,m*W);

def geomax(p):
	(n,m)=p;
	W=boxwidth();
	return geometry.Point((n+1)*W,(m+1)*W);

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

def neightbor8(p):
	(n,m)=p
	ret=set();	
	for k in [-1,0,1]:
		for l in [-1,0,1]:
			ret.add((n+k,m+l));
	return ret;

def rgb(color,colordict=None):
	if type(color) == type(0):
		c=color;
		return '#%02x%02x%02x' % (c,c,c);
	if type(color) == type((0,0,0)):
		return '#%02x%02x%02x' % color;
	if type(color) == type(None):
		return "green";
	if type(color) == type(set()):
		n=colordict[tuple(color)];
		M=len(colordict)
		f=int(256*n/M);
		if f<100:
			return '#%02x%02x%02x' % (f,0,0);
		elif f<200:
			return '#%02x%02x%02x' % (0,f,0);
		return '#%02x%02x%02x' % (0,0,f);
	print(type(color),color);
	assert(0);
	return

def plot_boxeset(boxset,filename,color=1):
	s=str();
	# set object 1 rect from 555243,5317261 to 555343,5317361 lw 1
	counter=1;
	drgb=rgb(color);
	for p in boxset:
		lrgb=drgb;
		Np=neightbor8(p);
		border=False;
		if not Np.issubset(boxset):
			border=True;	
		A=center(geomin(p));
		B=center(geomax(p));
		s+=f"set object {counter:d} rect from {A.x():d},{A.y():d} to {B.x():d},{B.y():d}\n"
		#if border:
			# set object 1 back clip linewidth 5 fc rgb "blue" fillstyle solid border lc rgb "black"
		s+=f"set object {counter:d} back clip lw 1 fc rgb \"white\" fillstyle solid border lc rgb \"black\"\n"
		#else:
		#s+=f"set object {counter:d} back clip lw 1 fc rgb \"{lrgb:s}\" fillstyle solid\n"
		counter = counter + 1;
	f=open(filename,'w');
	f.write(s);
	f.close();

def plot_boxesets(boxsets,colors,filename):
	s=str();
	# set object 1 rect from 555243,5317261 to 555343,5317361 lw 1
	counter=1;
	colordict=dict();
	if type(colors[0]) == type(set()):
		for c in colors:
			colordict[tuple(c)]=len(colordict);
	for k in range(len(boxsets)):
		drgb=rgb(colors[k],colordict);
		for p in boxsets[k]:
			lrgb=drgb;	
			Np=neightbor8(p);
			border=False;
			if not Np.issubset(boxsets[k]):
				border=True;	
			A=center(geomin(p));
			B=center(geomax(p));
			s+=f"set object {counter:d} rect from {A.x():d},{A.y():d} to {B.x():d},{B.y():d}\n"
			width=2;
			if True or border:
				# set object 1 back clip linewidth 5 fc rgb "blue" fillstyle solid border lc rgb "black"
				brgb=rgb(int(256*(k/len(boxsets))));
				s+=f"set object {counter:d} back clip lw {width:d} fc rgb \"{lrgb:s}\" fillstyle solid border lc rgb \"{brgb:s}\"\n"
			else:
				s+=f"set object {counter:d} back clip lw {width:d} fc rgb \"{lrgb:s}\" fillstyle solid\n"
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

def plot_boxes_and_tracks(indexset,tracks,bbox,filename):
	boxesfilename=filename+".segment";
	plot_boxeset(indexset,boxesfilename);
	plots=list();
	output=filename+".png";
	if ".gnuplot." in output:
		output=output.replace(".gnuplot","");
	tracksplot=make_tracksplot(tracks,filename);
	tmpl=open("plot.tmpl",'r').read();
	tmpl=tmpl.replace("{output}",output);
	tmpl=tmpl.replace("{boxesfilename}",boxesfilename);
	tmpl=tmpl.replace("{tracksplot}",tracksplot);
	(xmin,xmax,ymin,ymax)=bbox;
	margin=max([(xmax-xmin),(ymax-ymin)])*10;
	tmpl=tmpl.replace("{xmin}",str(geomin((xmin,ymin)).x()-margin));
	tmpl=tmpl.replace("{ymin}",str(geomin((xmin,ymin)).y()-margin));
	tmpl=tmpl.replace("{xmax}",str(geomin((xmax,ymax)).x()+margin));
	tmpl=tmpl.replace("{ymax}",str(geomin((xmax,ymax)).y()+margin));
	open(filename,'w').write(tmpl);


def plot_areas(areas,colors,tracks,bbox,filename):
	assert(len(areas)==len(colors));
	boxesfilename=filename+".segment";
	plot_boxesets(areas,colors,boxesfilename);
	output=filename+".png";
	tracksplot=make_tracksplot(tracks,filename);
	tmpl=open("plot.tmpl",'r').read();
	tmpl=tmpl.replace("{output}",output);
	tmpl=tmpl.replace("{boxesfilename}",boxesfilename);
	tmpl=tmpl.replace("{tracksplot}",tracksplot);
	(xmin,xmax,ymin,ymax)=bbox;
	margin=max([(xmax-xmin),(ymax-ymin)])*10;
	tmpl=tmpl.replace("{xmin}",str(geomin((xmin,ymin)).x()-margin));
	tmpl=tmpl.replace("{ymin}",str(geomin((xmin,ymin)).y()-margin));
	tmpl=tmpl.replace("{xmax}",str(geomin((xmax,ymax)).x()+margin));
	tmpl=tmpl.replace("{ymax}",str(geomin((xmax,ymax)).y()+margin));
	open(filename,'w').write(tmpl);

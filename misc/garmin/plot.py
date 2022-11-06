#!/usr/bin/env python3

import track
import geometry;

def center(point):
	(n,m)=(11138, 106351);
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
		A=center(boxes.min(p));
		B=center(boxes.max(p));
		s+=f"set object {counter:d} rect from {A.x():d},{A.y():d} to {B.x():d},{B.y():d} lw 1\n"
		counter = counter + 1;
	f=open(filename,'w');
	f.write(s);
	f.close();

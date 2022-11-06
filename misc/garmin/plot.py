#!/usr/bin/env python3

import track

def plot(track,filename):
	s=str();
	for p in track.geometry():
		s+=f"{p.x():f} {p.y():f}\n";
	f=open(filename,'w');
	f.write(s);
	f.close();
		

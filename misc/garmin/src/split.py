#!/usr/bin/env python3

class Pause:
	def __init__(self,start,end):
		self._start=start;
		selt._end=end;

def find_pauses(track):
	points=track.points();
	minspeed=3; # kmh
	moving=list();
	N=len(points);
	for n in range(len(points)-1):
		p0=points[n];
		p1=points[n+1];
		d=p0.distance(p1);
		dt=p1.time()-p0.time();
		if dt.total_seconds()==0:
			print(track.name());
			print(p0.string());
			print(p1.string());
			assert(0);
		mps=d/dt.total_seconds();
		kmh=3600*mps/1000;
		moving.append(kmh>minspeed);
	for m in 
		

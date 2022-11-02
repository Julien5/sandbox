#!/usr/bin/env python3

class Point:
	def __init__(self,latitude,longitude):
		self.latitude=latitude;
		self.longitude=longitude;
		self.x=None;
		self.y=None;
		self.elevation=None; # todo

	def string(self):
		width=8
		precision=7
		return f"({self.latitude:{width}.{precision}},{self.longitude:{width}.{precision}})"

class Track:
	def __init__(self,filename):
		self.filename=filename;
		self.points=dict();

	def append(self,time,Point):
		self.points[time]=Point;

	def string(self):
		ret=list();
		for time in self.points:
			t=time.strftime("%d.%m-%H:%M:%S");
			p=self.points[time].string();
			s=f"{t:s}:{p:s}"
			ret.append(s);
		return "\n".join(ret);	
	


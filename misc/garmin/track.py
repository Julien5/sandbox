#!/usr/bin/env python3

import math;
import datetime;

def sqr(x):
	return x*x;	

class Point:
	def __init__(self,latitude,longitude,x,y):
		self.latitude=latitude;
		self.longitude=longitude;
		self.x=x;
		self.y=y;
		self.elevation=None; # todo

	def string(self):
		width=8
		precision=7
		return f"({self.latitude:{width}.{precision}},{self.longitude:{width}.{precision}})"

	def distance(self,other):
		return math.sqrt(sqr(self.x - other.x) + sqr(self.y-other.y));

class Track:
	def __init__(self,filename):
		self._filename=filename;
		self._points=dict();

	def append(self,time,Point):
		self._points[time]=Point;

	def string(self):
		ret=list();
		for time in self.points:
			t=time.strftime("%d.%m-%H:%M:%S");
			p=self.points[time].string();
			s=f"{t:s}:{p:s}"
			ret.append(s);
		return "\n".join(ret);

	def points(self):
		return self._points

	def filename(self):
		return self._filename;

	def distance(self):
		d=0;
		times = list(self._points.keys());
		for k in range(len(times)-1):
			p1=self._points[times[k]];
			p2=self._points[times[k+1]];
			d+=p1.distance(p2);
		return d;

	def duration(self):
		if len(self._points)<2:
			return datetime.timedelta(0);
		times = sorted(list(self._points.keys()));
		return times[-1]-times[0];
		


	


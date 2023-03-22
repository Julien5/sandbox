#!/usr/bin/env

import math;
import datetime;

def sqr(x):
	return x*x;	

class Point:
	def __init__(self,x,y,latitude=None,longitude=None,elevation=0,time=None):
		self._latitude=latitude;
		self._longitude=longitude;
		self._x=x;
		self._y=y;
		self._elevation=elevation; # todo
		self._time=time;
	
	def string(self):
		if False and self.latitude:
			width=8;
			precision=7;
			return f"WGS84({self._latitude:{width}.{precision}},{self._longitude:{width}.{precision}})";
		return 	f"UTM({self.x():f},{self.y():f})";

	def distance(self,other):
		dx=self.x() - other.x();
		dy=self.y() - other.y();	
		return math.sqrt(sqr(dx) + sqr(dy));

	def x(self):
		return self._x;

	def y(self):
		return self._y;

	def latitude(self):
		return self._latitude;

	def longitude(self):
		return self._longitude;

	def elevation(self):
		return self._elevation;

	def time(self):
		return self._time;

def main():
	pass;	

if __name__ == '__main__':
	import sys;
	sys.exit(main())  
	

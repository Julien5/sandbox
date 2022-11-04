#!/usr/bin/env

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

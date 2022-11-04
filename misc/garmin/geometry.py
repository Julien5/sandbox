#!/usr/bin/env

import math;
import datetime;

def sqr(x):
	return x*x;	

class Point:
	def __init__(self,latitude,longitude,x,y):
		self.latitude=latitude;
		self.longitude=longitude;
		self._x=x;
		self._y=y;
		self.elevation=None; # todo

	def __init__(self,x,y):
		self.latitude=None;
		self.longitude=None;
		self._x=x;
		self._y=y;
		self.elevation=None; # todo
	

	def string(self):
		width=8
		precision=7
		return f"({self.latitude:{width}.{precision}},{self.longitude:{width}.{precision}})"

	def distance(self,other):
		return math.sqrt(sqr(self.x - other.x) + sqr(self.y-other.y));

	def x(self):
		return self._x;

	def y(self):
		return self._y;	

class Vector:
	def __init__(self,x,y):
		self._x = x;
		self._y = y;

	def string(self):
		width=3
		precision=1
		return f"({float(self.x()):{width}.{precision}},{float(self.y()):{width}.{precision}})"
		
	def fromPoints(p1,p2):
		x = p2.x() - p1.x();
		y = p2.y() - p1.y();
		return Vector(x,y);

	def x(self):
		return self._x;

	def y(self):
		return self._y;			

	def norm2(self):
		return self.product(self);

	def norm(self):
		return math.sqrt(self.norm2());

	def normalize(self):
		n=self.norm();
		self._x /= n;
		self._y /= n;

	def product(self,other):
		return self.x()*other.x()+self.y()*other.y();

	def add(self,other):
		return Vector(self.x()+other.x(),self.y()+other.y());

	def substract(self,other):
		return Vector(self.x()-other.x(),self.y()-other.y());

	def multiply(self,factor):
		return Vector(factor.self.x(),factor*self.y());

	def normal(self):
		if sqr(self.y())>0:
			ret=Vector(1,-self.x()/self.y());
			ret.normalize();
			return ret;
		return Vector(0,1);


def main():
	A=Point(3,2);
	B=Point(8,7);
	C=Point(3,5);
	D=Point(6,2);
	AB=Vector.fromPoints(A,B);
	CD=Vector.fromPoints(C,D);
	print(AB.product(CD));
	print(AB.norm2());
	AB.normalize();
	print(AB.norm2());
	CDn = CD.normal();
	print(CD.product(CDn));
	print(CD.string());
	print(CDn.string());

if __name__ == '__main__':
	import sys;
	sys.exit(main())  
	

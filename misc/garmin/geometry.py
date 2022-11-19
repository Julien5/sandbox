#!/usr/bin/env

import math;
import datetime;

def sqr(x):
	return x*x;	

class Point:
	def __init__(self,x,y,latitude=None,longitude=None):
		self.latitude=latitude;
		self.longitude=longitude;
		self._x=x;
		self._y=y;
		self.elevation=None; # todo
	
	def string(self):
		if False and self.latitude:
			width=8;
			precision=7;
			return f"WGS84({self.latitude:{width}.{precision}},{self.longitude:{width}.{precision}})";
		return 	f"UTM({self.x():f},{self.y():f})";
	

	def distance(self,other):
		dx=self.x() - other.x();
		dy=self.y() - other.y();	
		return math.sqrt(sqr(dx) + sqr(dy));

	def x(self):
		return self._x;

	def y(self):
		return self._y;	

class Vector:
	def __init__(self,x,y):
		self._x = x;
		self._y = y;

	def string(self):
		width=4
		precision=1
		return f"({float(self.x()):0{width}.{precision}f}, {float(self.y()):0{width}.{precision}f})"
		
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
		return Vector(factor*self.x(),factor*self.y());

	def normal(self):
		n=self.norm();
		ret=Vector(0,1);
		if sqr(self.y())>0:
			ret=Vector(1,-self.x()/self.y());
		ret.normalize();
		return ret.multiply(self.norm());

def distance(A,B,P):
	AB=Vector.fromPoints(A,B);
	AP=Vector.fromPoints(A,P);
	AC=AB.normal();
	xp = AB.product(AP)/AB.norm2();
	# yp = AC.product(AP)/AC.norm2();
	if 0 <= xp <= 1:
		# return yp*AC.norm();
		return abs(AC.product(AP)/AC.norm());
	if xp < 0:
		return AP.norm();
	BP=Vector.fromPoints(B,P).norm();
	return BP;

def lineparameters(A,B):
	dy=A.y()-B.y();
	dx=A.x()-B.x();
	if abs(dx)>1: # y=ax+b
		a=dy/dx;
		b=A.y() - a*A.x();
		return ['y',a,b];
	elif abs(dy)>1: # x=ay+b
		a=dx/dy;
		b=A.x() - a*A.y();
		return ['x',a,b];
	return [None,None,None];

def hit(A,B,U,V):
	Zero=Point(0,0);	
	AB=Vector.fromPoints(A,B);
	AC=AB.normal();
	
	AU=Vector.fromPoints(A,U);
	Ux = AB.product(AU)/AB.norm2();
	Uy = AC.product(AU)/AC.norm2();
	U2=Vector.fromPoints(Zero,Point(Ux,Uy));
	print("AB",AB.string());
	print("AC",AC.string());
	print("U2",U2.string());

	AV=Vector.fromPoints(A,V);
	Vx = AB.product(AV)/AB.norm2();
	Vy = AC.product(AV)/AC.norm2();
	V2=Vector.fromPoints(Zero,Point(Vx,Vy));
	print("V2",V2.string());
	
	epsilon=0.1
	if abs(U2.x() - V2.x())<epsilon:
		print("here");
		x=sum([U.x() for U in [U2,V2]])/2;
		if not (0 <= x <= 1):
			return False;
		miny=min([U.y() for U in [U2,V2]]);
		maxy=max([U.y() for U in [U2,V2]]);
		print("my",miny,maxy)
		if miny>0 or maxy<0:
			return False;
		return True;
	assert(False);
	return False	

def test3():
	A=Point(0,0)
	B=Point(1,0);
	for y in [0,0.5,1,2]:
		print("y",y);	
		U=Point(0.5,y);
		V=Point(0.5,y-1);
		if y<2:
			assert(hit(A,B,U,V));
		else:
			assert(not hit(A,B,U,V));	
	y=-1	
	C=Point(-1,y);
	D=Point(2,y);	
	assert(not hit(A,B,C,D));
	
def test1():
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

def test2():
	A=Point(3,2);
	B=Point(8,7);
	D=Point(6,2);
	print("distance:",distance(A,B,D));
	for x in range(10):
		D=Point(x,2+x);
		print(D.string(),"distance:",distance(A,B,D));
	
def main():
	#test1();	
	#test2();
	test3();

if __name__ == '__main__':
	import sys;
	sys.exit(main())  
	

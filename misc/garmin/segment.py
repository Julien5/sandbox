#!/usr/bin/env python3

import geometry;
import math;
import copy;
import plot;
import datetime;
import boxhitline;
import boxes

def boxwidth():
	return boxes.boxwidth();	

def hit3(n,m,u,v):
	return boxhitline.boxhitline(n,m,u,v);

def hit2(n,m,u,v):
	W=boxwidth();
	A=geometry.Point(n*W,m*W);
	B=geometry.Point((n+1)*W,m*W);
	C=geometry.Point((n+1)*W,(m+1)*W);
	D=geometry.Point(n*W,(m+1)*W);
	sides=[(A,B),(B,C),(C,D),(A,D)];
	for side in sides:
		(Al,Bl)=side;
		if geometry.hit(Al,Bl,u,v):
			return True;
	return False;

def hit(n,m,u,v):
	return hit3(n,m,u,v);

def main():
	pass;
		
if __name__ == '__main__':
	import sys;
	sys.exit(main())  

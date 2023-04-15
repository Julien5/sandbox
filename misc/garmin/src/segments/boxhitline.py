#!/usr/bin/env python3

import geometry;
from . import boxes;
import sys;

def W():
	return boxes.boxwidth();

def inside(xm,x,xM):
	return xm <= x <= xM;

def intersect(xm1,xM1,xm2,xM2):
	# TODO: check, make faster.
	in1=inside(xm1,xm2,xM1) or inside(xm1,xM2,xM1);
	in2=inside(xm2,xm1,xM2) or inside(xm2,xM1,xM2);
	return in1 or in2;

def bbox_intersect(n,m,u,v):
	xmin1=n*W();
	xmax1=(n+1)*W();
	ymin1=m*W();
	ymax1=(m+1)*W();
	xmin2=min([w.x() for w in [u,v]]);
	xmax2=max([w.x() for w in [u,v]]);
	ymin2=min([w.y() for w in [u,v]]);
	ymax2=max([w.y() for w in [u,v]]);
	intx=intersect(xmin1,xmax1,xmin2,xmax2);
	inty=intersect(ymin1,ymax1,ymin2,ymax2);
	return intx and inty;
	
def boxhitline_zero(n,m,u,v):
	assert(u.x()==v.x());
	x = sum([u.x(),v.x()])/2;

	xmin=n*W();
	xmax=(n+1)*W();
	ymin=m*W();
	ymax=(m+1)*W();
	
	# vertical hit test
	for xF in [xmin,xmax]:
		if x==xF:
			return True;

	# horizontal hit test
	return inside(xmin,x,xmax);

def boxhitline(n,m,u,v):
	if not bbox_intersect(n,m,u,v):
		return False;
	
	dx=v.x() - u.x();
	dy=v.y() - u.y();
	# get rid of the case dx=0.
	if dx==0:
		return boxhitline_zero(n,m,u,v);

	xmin=n*W();
	xmax=(n+1)*W();

	ymin=m*W();
	ymax=(m+1)*W();
	
	a=dy/dx;
	b=u.y() - a*u.x();

	# vertical hit test
	for xF in [xmin,xmax]:
		yF=a*xF+b;
		if inside(ymin,yF,ymax):
			return True;
		
	# horizontal hit test
	for yF in [ymin,ymax]:
		xF=(yF-b)/a;
		if inside(xmin, xF, xmax):
			return True;

def test():
	pp0=geometry.Point(555242.977,5317260.078)
	pp1=geometry.Point(555243.978,5317259.899)
	(pn,pm)=(11216, 106325)
	print(" pp0",pp0.string());
	print(" pp1",pp1.string());
	print("hit",boxhitline(pn,pm,pp0,pp1));

def main():
	test();	

if __name__ == '__main__':
	sys.exit(main())  
	

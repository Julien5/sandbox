#!/usr/bin/env python3

import geometry;
import math;
import copy;

def boundingbox(indexset):
	xmin=None;
	xmax=None;
	ymin=None;
	ymax=None;
	for p in indexset:
		(n,m)=p;
		if xmin is None:
			xmin=n;
			xmax=n;
		if ymin is None:
			ymin=m;
			ymax=m;
		xmin=min(xmin,n);
		xmax=max(xmax,n);
		ymin=min(ymin,m);
		ymax=max(ymax,m);
	return (xmin,xmax,ymin,ymax);

def hasintersection(bbox1,bbox2):
	(xmin1,xmax1,ymin1,ymax1)=bbox1;
	(xmin2,xmax2,ymin2,ymax2)=bbox2;
	xok=xmin1 <= xmin2 <= xmax1 or xmin2 <= xmin1 <= xmax2;
	yok=ymin1 <= ymin2 <= ymax1 or ymin2 <= ymin1 <= ymax2;
	return xok and yok;

class Segment:
	W=50
	def __init__(self,indexset,filenames):
		self.filenames=filenames;
		self._boxes=indexset;
		self._bbox=boundingbox(indexset);

	def geomin(self,p):
		(n,m)=p;
		return geometry.Point(n*self.W,m*self.W);

	def geomax(self,p):
		(n,m)=p;
		return geometry.Point((n+1)*self.W,(m+1)*self.W);

	def boxes(self):
		return self._boxes;

	def intersection(A,B):
		R=Boxes();
		if not hasintersection(A._bbox,B._bbox):
			return R;
		R.filenames=list();
		R.filenames.update(A.filenames);
		R.filenames.update(B.filenames);
		R._boxes=A._boxes.intersection(B._boxes);
		return R;

	def union(self,B):
		self.filenames.update(B.filenames);
		self._boxes=self._boxes.union(B._boxes);

	def distance(self,other):
		if not hasintersection(self._bbox,other._bbox):
			return 1;
		I=self._boxes.intersection(other._boxes);
		U=self._boxes.union(other._boxes);
		return len(U-I)/len(U);

	def merge(self,other):
		self._boxes=self._boxes.union(other._boxes);
		self.filenames.update(other.filenames);

def similars(seglist,seg):
	R=list();
	for k in range(len(seglist)):
		sk=seglist[k];
		if sk.distance(seg)<0.05:
			R.append(k); # break ?
			# break;
	return R;

def main():
	print("hello");

if __name__ == '__main__':
	import sys;
	sys.exit(main())  

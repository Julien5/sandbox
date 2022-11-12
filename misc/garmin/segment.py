#!/usr/bin/env python3

import geometry;
import math;
import copy;

class Segment:
	W=50
	def __init__(self,indexset,filenames):
		self.filenames=filenames;
		self._boxes=indexset;

	def min(self,p):
		(n,m)=p;
		return geometry.Point(n*self.W,m*self.W);

	def max(self,p):
		(n,m)=p;
		return geometry.Point((n+1)*self.W,(m+1)*self.W);

	def boxes(self):
		return self._boxes;

	def intersection(A,B):
		R=Boxes();
		R.filenames=list();
		R.filenames.update(A.filenames);
		R.filenames.update(B.filenames);
		R._boxes=A._boxes.intersection(B._boxes);
		return R;

	def union(self,B):
		self.filenames.update(B.filenames);
		self._boxes=self._boxes.union(B._boxes);

	def distance(self,other):
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

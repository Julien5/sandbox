#!/usr/bin/env python3

import geometry;
import math;
import copy;
import segment;

def hitlist(u,v):
	W=segment.boxwidth();	
	startn=math.floor(u.x()/W);
	stopn=math.floor(v.x()/W);
	stepn=1;
	if startn>stopn:
		stepn=-1;	
	startm=math.floor(u.y()/W);
	stopm=math.floor(v.y()/W);
	stepm=1;
	if startm>stopm:
		stepm=-1;
	A=u;#geometry.Point(xmin,ymin);
	B=v;#geometry.Point(xmax,ymax);
	ret=list();
	for n in range(startn,stopn+stepn,stepn):
		for m in range(startm,stopm+stepm,stepm):
			if segment.hit(n,m,A,B):
				ret.append((n,m));
	return ret;

def hitset(u,v):
	return set(hitlist(u,v));	

class Boxes:
	W=segment.boxwidth();
	def __init__(self,track=None):
		self.tracks=set();
		if track:
			self.tracks=set([track]);
		self._boxes=set();

	def min(self,p):
		(n,m)=p;
		return geometry.Point(n*self.W,m*self.W);

	def max(self,p):
		(n,m)=p;
		return geometry.Point((n+1)*self.W,(m+1)*self.W);

	def add(self,u,v):
		S=hitset(u,v);
		for p in S:
			self._boxes.add(p);

	def addsurroundings(self,d):
		# add the d-surroundings
		K=math.ceil(d/(self.W*math.sqrt(2)));
		A=set();
		for b in self._boxes:
			(n,m)=b;
			for kx in range(-K,(K+1)):
				for ky in range(-K,(K+1)):
					A.add((n+kx,m+ky));
		for a in A:
			self._boxes.add(a);
			
	def boxes(self):
		return self._boxes;

	def intersection(A,B):
		R=Boxes();
		R.tracks=set();
		R.tracks.update(A.tracks);
		R.tracks.update(B.tracks);
		R._boxes=A._boxes.intersection(B._boxes);
		return R;

	def union(self,B):
		self.tracks.update(B.tracks);
		self._boxes=self._boxes.union(B._boxes);

	def distance(self,other):
		I=self._boxes.intersection(other._boxes);
		U=self._boxes.union(other._boxes);
		return len(U-I)/len(U);


def boxes(track):
	B=Boxes(track);
	G=track.geometry();
	for k in range(len(G)-1):
		u=G[k];
		v=G[k+1];
		B.add(u,v);
	B.addsurroundings(50);	
	return B._boxes;

def main():
	B=Boxes();
	W=B.W;
	
	u=geometry.Point(555451.717571,5317244.658870);
	v=geometry.Point(555486.123428,5317205.641802);
	L=hitlist(u,v);
	assert(L);
	
	(n1,m1)=(-2,-2);
	(n2,m2)=(2,2);
	u=geometry.Point(n1*W,m1*W);
	v=geometry.Point(n2*W,m2*W);
	print(hitlist(u,v));


if __name__ == '__main__':
	import sys;
	sys.exit(main())  

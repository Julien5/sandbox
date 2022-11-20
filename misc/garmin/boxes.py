#!/usr/bin/env python3

import geometry;
import math;
import copy;
import segment;

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
		xmin=min(u.x(),v.x());
		xmax=max(u.x(),v.x());
		ymin=min(u.y(),v.y());
		ymax=max(u.y(),v.y());
		Nmin=math.floor(xmin/self.W);
		Nmax=math.floor(xmax/self.W);
		Mmin=math.floor(ymin/self.W);
		Mmax=math.floor(ymax/self.W);
		A=u;#geometry.Point(xmin,ymin);
		B=v;#geometry.Point(xmax,ymax);
		for n in range(Nmin-1,Nmax+2):
			for m in range(Mmin-1,Mmax+2):
				if segment.hit(n,m,A,B):
					self._boxes.add((n,m));

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

	def neighbours(self,a):
		(n,m)=a;
		R=set();
		for k in [-1,0,+1]:
			for l in [-1,0,+1]:
				R.add((n+k,m+l));
		return R;		

	def segments(self):
		R=list();
		A=copy.deepcopy(self._boxes);
		C=set();
		while A:
			Q=None;
			for a in A:
				Q=[a];
				break;
			if not Q:
				return;
			assert(Q);
			while Q:
				a=Q.pop(0);
				for n in A.intersection(self.neighbours(a)):
					if not n in C:
						C.add(n); # label
						Q.append(n); # add to the queue
			R.append(copy.deepcopy(C));
			A=A-C;
			C.clear();
		RB=list();	
		for r in R:
			if len(r)<200:
				continue;	
			B=segment.Segment(r,self.tracks);
			RB.append(B);
		return RB;

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
	return B;

def main():
	B=Boxes();
	W=B.W;
	(n,m)=(0,0);
	
	u=geometry.Point(0,-W);
	v=geometry.Point(W/2,2*W);
	[mode,a,b]=geometry.lineparameters(u,v);
	assert(B.boxhitlineparameters([mode,a,b],n,m));

	u=geometry.Point(-W,W/2);
	v=geometry.Point(W,W/2);
	[mode,a,b]=geometry.lineparameters(u,v);
	assert(B.boxhitlineparameters([mode,a,b],n,m));

	u=geometry.Point(-W,W/2)
	v=geometry.Point(W,W/2);
	[mode,a,b]=geometry.lineparameters(u,v);
	assert(B.boxhitlineparameters([mode,a,b],n,m));

	(n,m)=(11138, 106351);
	u=geometry.Point( 556912.7-n*W,5317640.2-m*W)
	v=geometry.Point( 557012.5-n*W,5317514.1-m*W)
	print(u.string());
	print(v.string());
	(n,m)=(0,0);
	[mode,a,b]=geometry.lineparameters(u,v);
	assert(B.boxhitlineparameters([mode,a,b],n,m));

if __name__ == '__main__':
	import sys;
	sys.exit(main())  

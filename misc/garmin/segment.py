#!/usr/bin/env python3

import geometry;
import math;
import copy;
import plot;
import datetime;
import boxhitline;

def boxwidth():
	return 50	

def geomin(p):
	(n,m)=p;
	W=boxwidth();
	return geometry.Point(n*W,m*W);

def geomax(p):
	(n,m)=p;
	W=boxwidth();
	return geometry.Point((n+1)*W,(m+1)*W);

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

def boxcontainspoint(n,m,u):
	minpoint=geomin((n,m));
	maxpoint=geomax((n,m));
	xok = minpoint.x() <= u.x() <= maxpoint.x();
	yok = minpoint.y() <= u.y() <= maxpoint.y();
	return xok and yok;

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
	#print("d",xmax-xmin)		
	return (xmin,xmax,ymin,ymax);

def hasintersection(bbox1,bbox2):
	(xmin1,xmax1,ymin1,ymax1)=bbox1;
	(xmin2,xmax2,ymin2,ymax2)=bbox2;
	xok=xmin1 <= xmin2 <= xmax1 or xmin2 <= xmin1 <= xmax2;
	yok=ymin1 <= ymin2 <= ymax1 or ymin2 <= ymin1 <= ymax2;
	return xok and yok;

class Segment:
	W=50
	def __init__(self,indexset,tracks):
		self.tracks=tracks;
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

	def bbox(self):
		return self._bbox;

	def intersection(A,B):
		R=Boxes();
		if not hasintersection(A._bbox,B._bbox):
			return R;
		R.tracks=list();
		R.tracks.update(A.tracks);
		R.tracks.update(B.tracks);
		R._boxes=A._boxes.intersection(B._boxes);
		return R;

	def union(self,B):
		self.tracks.update(B.tracks);
		self._boxes=self._boxes.union(B._boxes);

	def distance(self,other):
		if not hasintersection(self._bbox,other._bbox):
			return 1;
		I=self._boxes.intersection(other._boxes);
		U=self._boxes.union(other._boxes);
		return len(U-I)/len(U);

	def containsline(self,u,v):
		for b in self._boxes:
			(n,m)=b;
			if hit(n,m,u,v):
				return (n,m)
		return None;

	def containspoint(self,u):
		nx=math.floor(u.x()/boxwidth());
		my=math.floor(u.y()/boxwidth());
		return (nx,my) in self._boxes;

	def length_along_track(self,track):
		if not self.tracks:
			raise Exception("no track to compute length");
		first=None;
		last=None;
		P=track.points();
		times=sorted(list(P.keys()));
		assert(len(times)==len(P));
		ret=None;
		rets=[];
		for k in range(len(times)-1):
			t0=times[k];
			t1=times[k+1];
			p0=P[t0];
			p1=P[t1];
			if not first and self.containspoint(p1):
				first=t1;
				ret=0;
				continue;
			if first and self.containspoint(p1):
				ret += p0.distance(p1);
				if k<len(times)-2:
					continue;
			if first and not last:
				last=t0;
				rets.append([ret,first,last]);
				ret=None;
				first=None;
				last=None;
		rmax=None;
		if not rets:
			assert(0);	
			plot.plot_boxes_and_tracks(self,self.tracks,"/tmp/debug-0.gnuplot");
			return None;	
		M=max([r[0] for r in rets]);
		for r in rets:
			if r[0]	== M:
				rmax=r;
				break;
		[ret,first,last]=rmax;
		debug=False;
		if debug:
			d = datetime.timedelta(minutes=0)
			subtrack=track.subtrack(first,last);
			tracks=[track,subtrack];
			plot.plot_boxes_and_tracks(self,tracks,"/tmp/debug-{}.gnuplot".format(track.name()))
			if (not first) or (not last) or (first == last):
				return None;
		assert(ret>0);
		return ret;

	def length(self):
		if not self.tracks:
			raise Exception("no track to compute length");
		lengths=list();
		for t in self.tracks:
			L=self.length_along_track(t);	
			if not (L is None):	
				lengths.append(L);
		#print(lengths);
		if not lengths:
			print("what?!?");	
			return -1;	
		return max(lengths);	

	def merge(self,other):
		self._boxes=self._boxes.union(other._boxes);
		self.tracks.update(other.tracks);

def similars(seglist,seg):
	R=list();
	for k in range(len(seglist)):
		sk=seglist[k];
		if sk.distance(seg)<0.05:
			R.append(k); # break ?
			# break;
	return R;


def test():
#(11218, 106332)
# True
# p0 UTM( 555243.2,5317250.8)
# p1 UTM( 555241.9,5317250.9)
	pp0=geometry.Point(555242.977,5317260.078)
	pp1=geometry.Point(555243.978,5317259.899)
	(pn,pm)=(11216, 106325)
	print("bmin",geomin((pn,pm)).string());
	print("bmax",geomax((pn,pm)).string());
	print(" pp0",pp0.string());
	print(" pp1",pp1.string());
	print("hit",boxhitline(pn,pm,pp0,pp1));


def main():
	test();	

if __name__ == '__main__':
	import sys;
	sys.exit(main())  

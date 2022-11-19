#!/usr/bin/env python3

import geometry;
import math;
import copy;
import plot;
import datetime;

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


def boxhitlineparameters(line,n,m):
	[mode,a,b]=line;
	W=boxwidth();
	if mode == "x": # vertical
		for k in range(m,m+2):
			yF=k*W;
			xF=a*yF+b;
			if n*W <= xF <= (n+1)*W:
				return True;
		if a == 0:
			assert(False);
			return False;
		for k in range(n,n+2):
			xF=k*W;
			yF=(xF-b)/a;
			if m*W <= yF <= (m+1)*W:
				return True;
		return False;	
	if mode == "y": # vertical
		for k in range(n,n+2):
			xF=k*W;
			yF=a*xF+b;
			if m*W <= yF <= (m+1)*W:
				print("hit",k-n,k,xF,m*W, yF, (m+1)*W)
				print("hity");	
				return True;
		if a == 0:
			assert(False);
			return False;
		for k in range(m,m+2):
			yF=k*W;
			xF=(yF-b)/a;
			if n*W <= xF <= (n+1)*W:
				print("hitx");		
				return True;
		return False;
	assert(False);
	return False;

def boxhitline(n,m,u,v):
	[mode,a,b]=geometry.lineparameters(u,v);
	print([mode,a,b]);
	if not mode:
		return False;
	return boxhitlineparameters([mode,a,b],n,m);

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
			if boxhitline(n,m,u,v):
				return (n,m)
		return None;

	def containspoint(self,u):
		for b in self._boxes:
			(n,m)=b;
			if boxcontainspoint(n,m,u):
				return (n,m);
		return None;

	def length_along_track(self,track):
		if not self.tracks:
			raise Exception("no track to compute length");
		pprev=None;
		first=None;
		last=None;
		P=track.points();
		times=sorted(list(P.keys()));
		assert(len(times)==len(P));
		ret=0;
		for k in range(len(times)-1):
			t0=times[k];
			t1=times[k+1];
			p0=P[t0];
			p1=P[t1];
			print(">>> ",k,t0,"first:",first,"last:",last,"ret:",ret)
			index=self.containsline(p0,p1)
			if not first and not (index is None):
				print(index);
				(n,m)=index;
				assert(boxhitline(n,m,p0,p1))
				print("p0",p0.string());
				print("p0.x",p0.x())
				print("p1",p1.string());
				pp0=geometry.Point(555242.977628,5317260.078650)
				pp1=geometry.Point(555243.978894,5317259.899306)
				(pn,pm)=(11216, 106325)
				# (11216, 106325)
				#p0 UTM( 555243.0,5317260.1)
				#p1 UTM( 555244.0,5317259.9)
				if (pn,pm) == (n,m): # and (pp0,pp1)==(p0,p1):
					print("hit",boxhitline(pn,pm,pp0,pp1));
					assert(False)
				return None;
			if not first and self.containspoint(p0):
				first=t0;
				ret=0;
				continue;
			if first and self.containspoint(p1):
				ret += p0.distance(p1);
				continue;
			if first and not last and not self.containspoint(p1):
				last=t0;
			if first and last:
				print(track.name(),first,last)
				break;
		print("RESULT ",first,last,ret);
		d = datetime.timedelta(minutes=0)
		subtrack=track.subtrack(first-d,last+d);
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
		print(lengths);
		if not lengths:
			return -1;	
		return min(lengths);	

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


def 	


def main():
	test();	

if __name__ == '__main__':
	import sys;
	sys.exit(main())  

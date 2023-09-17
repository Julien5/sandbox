#!/usr/bin/env python3

import datetime;
import copy;
import math;
class Track:
	def __init__(self,name,points=None):
		self._name=name;
		self._points=list();
		self._cache=dict();
		if points:
			self._points=points;

	def sanity_check(self):
		G=self.points();
		L=len(G);
		maxdelta=0;
		assert(L);
		for k in range(L-1):
			p0=G[k];
			p1=G[k+1];
			delta=p1.time()-p0.time();
			maxdelta=max(delta.total_seconds(),maxdelta);
			if delta.total_seconds()>3600:
				return False;
		return True;
	
	def strip(self):
		G=list(self.points());
		self._cache.clear();
		startpoint = G[0];
		stoppoint = G[-1];
		threshold=50
		while(G and startpoint.distance(G[0])<threshold):
   			G.pop(0);
		while(G and stoppoint.distance(G[-1])<threshold):
			G.pop(-1);
		#n=len(self._points)-len(G);
		#if n>0:
		#	print("removed",n,"points from",len(self._points))
		self._points=G;


	def empty(self):
		return len(self._points)==0;

	def append(self,p):
		self._points.append(p);
		self._cache.clear();

	def append_subtrack(self,subtrack):
		self._points.extend(subtrack._points);
		self._cache.clear();

	def string(self):
		ret=list();
		for k in range(len(self._points)):
			point=self._points[k];
			time=point.time();
			t=time.strftime("%Y.%m.%d-%H:%M:%S");
			p=point.string();
			s=f"{t:s}:{p:s}"
			ret.append(s);
		return "\n".join(ret);

	def speed(self):
		if not self.duration().total_seconds():
			return None;	
		return self.distance()/self.duration().total_seconds();

	def moving_speed(self):
		minspeed=4*1000.0/3600;
		G=self.points()
		N=len(G);
		Dx=0;
		Dt=0;
		for n in range(N-1):
			p1=G[n];
			p2=G[n+1];
			dx=p1.distance(p2);
			dt=(p2.time()-p1.time()).total_seconds();
			speed=dx/dt;
			if speed>minspeed:
				Dx+=dx;
				Dt+=dt;
		return Dx/Dt;
	
	def category(self):
		assert(self._points);	
		day=self._points[0].time().strftime("%Y-%m-%d");
		if self.distance()<1000:
			return "none";	
		minrunningspeed=1*1000/3600;
		maxrunningspeed=15*1000/3600;
		if not self.speed():
			return "none";
		if self.speed()<=minrunningspeed:
			return "none";
		if self.speed()>maxrunningspeed:
			return "cycling";
		if self.distance()>30000:
			return "cycling";
		if self.moving_speed()>maxrunningspeed:
			return "cycling";
		return "running";

	def points(self):
		return self._points;

	def subtrack(self,k0,kend):
		ret=Track(self._name);
		ret._name+=".sub";
		P=self.points();
		K=range(len(P));
		for k in K:
			if k<k0 or k>=kend:
				continue;
			ret.append(P[k]);
		return ret;

	def subtrack_time(self,time_start,time_end):
		ret=Track(self._name);
		P=sorted(self.points(), key=lambda x: x.time)
		for p in P:
			if time_start <= p.time() <= time_end:
				ret.append(p);
		return ret;

	def bbox(self):
		P=self.geometry();	
		xmin=min([p.x() for p in P]);
		xmax=max([p.x() for p in P]);
		ymin=min([p.y() for p in P]);
		ymax=max([p.y() for p in P]);
		return (xmin,xmax,ymin,ymax);

	def geometry(self):
		return copy.deepcopy(self._points);

	def name(self):
		return self._name;

	def distance(self):
		if not "distance" in self._cache:
			d=0;
			for k in range(len(self._points)-1):
				p1=self._points[k];
				p2=self._points[k+1];
				delta=p1.distance(p2);
				d+=delta;
			self._cache["distance"]=d;
		return self._cache["distance"];

	def times(self):
		if not "times" in self._cache:
			self._cache["times"]=[p.time() for p in self._points];
		return self._cache["times"];

	def duration(self):
		if not "duration" in self._cache:
			T=self.times();
			self._cache["duration"]=max(T)-min(T);
		return self._cache["duration"];	

	def begintime(self):
		T=self.times();
		if not T[0]:
			print(self.name());
		assert(T[0]);
		return min(T);

	def endtime(self):
		T=self.times();
		return max(T);

	def __hash__(self):
		return hash((self._name))

	def __eq__(self, other):
		if not isinstance(other, type(self)):
				return NotImplemented
		return self._name == other._name;

	def __lt__(self, other):
		return self._name < other._name

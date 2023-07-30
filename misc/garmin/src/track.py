#!/usr/bin/env python3

import datetime;
import copy;
import math;
class Track:
	def __init__(self,name,points=None):
		self._name=name;
		self._points=list();
		if points:
			self._points=points;

	def strip(self):
		G=self.points();
		startpoint = G[0];
		stoppoint = G[-1];
		threshold=50
		while(G and startpoint.distance(G[0])<threshold):
   			G.pop(0);
		while(G and stoppoint.distance(G[-1])<threshold):
			G.pop(-1);
		self._points=G;

	def empty(self):
		return len(self._points)==0;

	def append(self,p):
		self._points.append(p);

	def time_sort(self):
		self._points=sorted(self.points(), key=lambda x: x.time)

	def append_subtrack(self,subtrack):
		self._points.extend(subtrack._points);

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

	def category(self):
		assert(self._points);	
		day=self._points[0].time().strftime("%Y-%m-%d");
		if self.distance()<1000:
			return "none";	
		# en velo avec les enfants
		if day=="2022-08-20":
			return "cycling";
		# en velo au tennis
		if day=="2022-11-17":
			return "cycling";
		minrunningspeed=1*1000/3600;
		maxrunningspeed=15*1000/3600;
		if not self.speed():
			return "none";
		if self.speed()<=minrunningspeed:
			return "none";
		if self.distance()<=100:
			return "none";	
		if self.speed()>maxrunningspeed:
			return "cycling";
		if self.distance()>30000:
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
		d=0;
		for k in range(len(self._points)-1):
			p1=self._points[k];
			p2=self._points[k+1];
			delta=p1.distance(p2);
			d+=delta;
		return d;

	def times(self):
		return [p.time() for p in self._points];

	def duration(self):
		T=self.times();
		return max(T)-min(T);

	def begintime(self):
		T=self.times();
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

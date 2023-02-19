#!/usr/bin/env python3

import datetime;
import copy;
import math;
class Track:
	def __init__(self,name):
		self._name=name;
		self._points=dict();

	def append(self,time,p):
		self._points[time]=p;

	def string(self):
		ret=list();
		for time in self._points:
			t=time.strftime("%Y.%m.%d-%H:%M:%S");
			p=self._points[time].string();
			s=f"{t:s}:{p:s}"
			ret.append(s);
		return "\n".join(ret);

	def speed(self):
		if not self.duration().total_seconds():
			return None;	
		return self.distance()/self.duration().total_seconds();

	def category(self):
		assert(self._points);	
		day=sorted(self._points)[0].strftime("%Y-%m-%d");
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
		times=sorted(self._points.keys());
		K=range(len(P));
		for k in K:
			if k<k0 or k>=kend:
				continue;
			t=times[k];
			ret.append(t,P[t]);
		return ret;

	def subtrack_time(self,time_start,time_end):
		ret=Track(self._name);
		P=self.points();
		for t in P:
			if time_start <= t <= time_end:
				ret.append(t,P[t]);
		return ret;

	def bbox(self):
		P=self.geometry();	
		xmin=min([p.x() for p in P]);
		xmax=max([p.x() for p in P]);
		ymin=min([p.y() for p in P]);
		ymax=max([p.y() for p in P]);
		return (xmin,xmax,ymin,ymax);

	def geometry(self):
		P=self._points;
		return [P[t] for t in sorted(P.keys())];

	def name(self):
		return self._name;

	def distance(self):
		d=0;
		times = list(self._points.keys());
		for k in range(len(times)-1):
			p1=self._points[times[k]];
			p2=self._points[times[k+1]];
			delta=p1.distance(p2);
			d+=delta;
		return d;

	def duration(self):
		if len(self._points)<2:
			return datetime.timedelta(0);
		times = sorted(list(self._points.keys()));
		return times[-1]-times[0];

	def begintime(self):
		times = sorted(list(self._points.keys()));
		return times[0];	

	def endtime(self):
		times = sorted(list(self._points.keys()));
		return times[-1];

	def __hash__(self):
		return hash((self._name))

	def __eq__(self, other):
		if not isinstance(other, type(self)):
				return NotImplemented
		return self._name == other._name;

	def __lt__(self, other):
		return self._name < other._name

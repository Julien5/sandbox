#!/usr/bin/env python3

import datetime;
import copy;

class Track:
	def __init__(self,name):
		self._name=name;
		self._points=dict();

	def append(self,time,p):
		self._points[time]=p;

	def string(self):
		ret=list();
		for time in self.points:
			t=time.strftime("%d.%m-%H:%M:%S");
			p=self.points[time].string();
			s=f"{t:s}:{p:s}"
			ret.append(s);
		return "\n".join(ret);

	def points(self):
		return self._points;

	def subtrack(self,k0,kend):
		ret=Track(self._name);
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

	def __hash__(self):
		return hash((self._name))

	def __eq__(self, other):
		if not isinstance(other, type(self)):
				return NotImplemented
		return self._name == other._name;

	def __lt__(self, other):
		return self._name < other._name

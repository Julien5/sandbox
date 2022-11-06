#!/usr/bin/env python3

import datetime;

class Track:
	def __init__(self,filename):
		self._filename=filename;
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

	def geometry(self):
		return self._points.values();

	def filename(self):
		return self._filename;

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
	 

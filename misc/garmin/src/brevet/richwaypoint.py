#!/usr/bin/env python3

class RichWaypoint:
	def __init__(self,point):
		self.point=point;
		self.name="";
		self.description="";
		self.distance=None;
		self.time=None;
		self.label_on_profile=True;
		self.slope=None;
		self.dplus=None;
		self.xdplus=0;
		self.type=None;

	def isControlPoint(self):
		return self.type == "K";

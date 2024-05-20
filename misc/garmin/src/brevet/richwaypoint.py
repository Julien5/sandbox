#!/usr/bin/env python3

class RichWaypoint:
	def __init__(self,point):
		self.point=point;
		self.name="";
		self.description="";
		self.distance=None;
		self.index=None;
		self.time=None;
		self.hide=False;
		self.label_on_profile=True;
		self.slope=None;
		self.dplus=None;
		self.xdplus=0;
		self.type=None;

	def isControlPoint(self):
		return self.type == "K";

	def isAutomatic(self):
		return self.type == "A";

	def __eq__(self, other): 
		if not isinstance(other, RichWaypoint):
			return NotImplemented
		if self.index != other.index:
			return False;
		if self.type != other.type:
			return False;
		if self.name != other.name:
			return False;
		# to be continued...
		return True;

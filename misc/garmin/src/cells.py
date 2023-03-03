#!/usr/bin/env python3

import sys

class Cell:
	def __init__(self,area,color):
		assert(type(color)==type(set()));	
		self._area=area;
		self._color=set(color);

	def area(self):
		return self._area;

	def color(self):
		return self._color;

class Segment:
	def __init__(self,indices):
		self._indices=indices;

	def bigCell(self,Cells):
		return union([Cells[k] for k in self._indices]);

	def indices(self):
		return self._indices;

class SegmentsDictionary:
	def __init__(self,result):
		# result: color -> indices
		# example: R[(1)] = {{1,3,4},{6}}
		self._result=result;

	def segments(self,color):
		G=self._result[color];
		return [Segment(g) for g in G];

	def color(self,segment):
		I=segment.indices();
		for color in self._result:
			# "in" and not "==" because self._result[color] contains
			# multiple segments
			if I in self._result[color]:
				return color;
		assert(0);
		return None;

	def colors(self):
		return list(self._result.keys());

	def all_segments(self):
		ret=set();
		for color in result:
			ret.add(self.segments(color));
		return ret;

	#def colors(self):
	#	return list(self._result.keys());


def color(Cells):
	if len(Cells)==1:
		return Cells[0].color();
	colors=[set(c.color()) for c in Cells];
	color=colors[0].intersection(*colors);
	return color;

def union(Cells):
	if len(Cells)==1:
		return list(Cells)[0];
	areas=[c.area() for c in Cells];
	area=set().union(*areas);
	return Cell(area,color(Cells));

def main():
	return None;	
	
if __name__ == '__main__':
	sys.exit(main())  

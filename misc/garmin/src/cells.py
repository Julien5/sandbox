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

def color(Cells):
	if len(Cells)==1:
		return Cells[0].color();
	colors=[set(c.color()) for c in Cells];
	color=colors[0].intersection(*colors);
	return color;

def union(Cells):
	if len(Cells)==1:
		return Cells[0];
	areas=[c.area() for c in Cells];
	area=set().union(*areas);
	return Cell(area,color(Cells));

def main():
	return None;	
	
if __name__ == '__main__':
	sys.exit(main())  

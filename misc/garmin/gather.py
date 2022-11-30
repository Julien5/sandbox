#!/usr/bin/env python3

import sys
import cells

class ColorAccumulator:
	def __init__(self):
		self.domain = dict();
		
	def __call__(self, _union, _color):
		print("visit:",_union,"color",_color);
		union=tuple(sorted(_union));
		color=tuple(sorted(_color));
		if not color in self.domain:
			self.domain[color]=set();
			
		filtered=set();
		for s in self.domain[color]:
			# A <= B means A included in B
			# not: s is a tuple.
			if not (set(s) <= _union):
				filtered.add(s);
		filtered.add(union);		
		self.domain[color]=filtered;

	def result(self):
		return self.domain;	

def gather(C,childs,functor):
	unions=list();
	for k in range(len(childs)-1):
		s1=childs[k];
		s2=childs[k+1];
		U=s1.union(s2);
		color=cells.color([C[k] for k in U]);
		if color:
			functor(U,color);
			unions.append(U);
	if unions:
		gather(C,unions,functor);

def main():
	area=set();
	C=list();
	C.append(cells.Cell(area,{1,2}));
	C.append(cells.Cell(area,{1,2,3}));
	C.append(cells.Cell(area,{2,3}));
	C.append(cells.Cell(area,{3,4}));
	acc=ColorAccumulator();
	gather(C,[{k} for k in range(len(C))],acc);
	R=acc.result();
	for color in R:
		ret="|".join([str(set(g)) for g in R[color]]);
		print("groups for ",set(color),"are:",ret);
			
	
if __name__ == '__main__':
	sys.exit(main())  

#!/usr/bin/env python3

import sys
import cells

def element(s):
	assert(len(s)==1);
	for k in s:
		return k;
	assert(0);
	return None;

def rootcolor(C):
	return cells.color(C);
	#return cells.union(C).color();

class Accumulator:
	def __init__(self):
		self.domain = dict();
		
	def __call__(self, _union, _color):
		# print("visit:",_union,"color",_color);
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

	def print(self):
		R=self.result();
		for color in R:
			ret="|".join([str(set(g)) for g in R[color]]);
			print("groups for",set(color),"are:",ret);

	def check(self,C,L):
		R=self.result();
		subC=[C[element(k)] for k in L];
		root=tuple(sorted(rootcolor(subC)));
		if not root:
			print("[empty root color]");
			return False;
		if not root in R:
			print("[root color not found]");
			return False;
		if not tuple(sorted(set().union(*L))) in R[root]:
			print("[top not contain all]");
			return False;
		return True;

def walk(C,childs,functor):
	unions=list();
	for k in range(len(childs)):
		functor(childs[k],cells.color([C[l] for l in childs[k]]));
		
	for k in range(len(childs)-1):
		s1=childs[k];
		s2=childs[k+1];
		U=s1.union(s2);
		color=cells.color([C[k] for k in U]);
		if color:
			unions.append(U);
			
	if unions:
		walk(C,unions,functor);

def test1():
	area=set();
	C=list();
	index=7;
	C.append(cells.Cell(area,{1,2,index}));
	C.append(cells.Cell(area,{1,2,3,index}));
	C.append(cells.Cell(area,{2,3,index}));
	C.append(cells.Cell(area,{3,4,index}));
	acc=Accumulator();
	L=[{k} for k in range(len(C))];
	walk(C,L,acc);
	acc.print()
	assert(acc.check(C,L));

def test2():
	area=set();
	C=list();
	index=7;
	index2=14;
	C.append(cells.Cell(area,{1,2,index,index2}));
	C.append(cells.Cell(area,{2,3,index,index2}));
	C.append(cells.Cell(area,{3,4,index,index2}));
	C.append(cells.Cell(area,{4,2,index,index2}));
	C.append(cells.Cell(area,{2,5,index,index2}));
	acc=Accumulator();
	L=[{k} for k in range(len(C))];
	walk(C,L,acc);
	acc.print();
	assert(acc.check(C,L));

def main():
	test2();
	#test2();
		
	
if __name__ == '__main__':
	sys.exit(main())  

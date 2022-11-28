#!/usr/bin/env python3

import sys

class Cell:
	def __init__(self,area,color):
		self._area=area;
		self._color=color;

	def area(self):
		return self._area;

	def bbox(self):
		nmin=min([n for (n,m) in self._area]);
		nmax=max([n for (n,m) in self._area])+1;
		mmin=min([m for (n,m) in self._area]);
		mmax=max([m for (n,m) in self._area])+1;	
		return (nmin,nmax,mmin,mmax);

	def color(self):
		return self._color;

def cleanup(Cells):
	ret=list();	
	for c in Cells:
		if len(c.area())<10:
			continue;
		if len(c.color())<2:
			continue;
		ret.append(c);
	return ret;	

def union(Cells):
	if len(Cells)==1:
		return Cells[0];
	colors=[set(c.color()) for c in Cells];
	color=set().intersection(*colors);
	areas=[c.area() for c in Cells];
	area=set().union(*areas);
	return Cell(area,color);

# mother cells contain all there neighboors	
def mothers(cells,neighboorsmap):
	R=range(len(cells));
	mothers=set();
	for k in R:
		ismother=True;
		mothercolor=set(cells[k].color());
		for n in neighboorsmap[k]:
			neighboor=cells[n];
			color_neighboor=set(neighboor.color())
			if not color_neighboor.issubset(mothercolor):
				ismother=False;
				break;
		if ismother:
			mothers.add(k);
	return mothers;

def gathercolor(cells,mother_set,container):
	color=union([cells[k] for k in mother_set]);
	container.add(color);

class ColorAccumulator:
	def __init__(self,C):
		self._container = set();
		self.C=C;
		
	def __call__(self, mother_set):
		U=union([self.C[k] for k in mother_set]);
		if len(U.color())>0:
			self._container.add(tuple(U.color()));

	def container(self):
		return self._container;	

# a (sub)mother is a union of cells.				
def walk(mother_set,neighboorsmap,functor):
	if type(mother_set) is type(0):
		mother_set={mother_set};	
	functor(mother_set);
	neighboors=set().union(*[neighboorsmap[k] for k in mother_set])-mother_set;
	for n in neighboors:
		submother_set=mother_set.union({n});
		walk(submother_set,neighboorsmap,functor);
		

# now the question is: how many cell do we have, which
# (1) are not mothers, and
# (2) have no mother.
# That is the question.

def zombies(cells,mothers,neighboors):
	return None;	
	
if __name__ == '__main__':
	sys.exit(main())  

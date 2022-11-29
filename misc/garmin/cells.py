#!/usr/bin/env python3

import sys

class Cell:
	def __init__(self,area,color):
		assert(type(color)==type(set()));	
		self._area=area;
		self._color=set(color);

	def area(self):
		return self._area;

	def distance(self,other):
		Inter=self.area().intersection(other.area());
		Union=self.area().union(other.area());
		return len(Union-Inter)/len(Union);
	
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
		if len(c.area())<=3:
			continue;
		#if len(c.color())<2:
		#	continue;
		ret.append(c);
	return ret;	

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

def uunion(Cells):
	if len(Cells)==1:
		return Cells[0];
	areas=[c.area() for c in Cells];
	area=set().union(*areas);
	colors=[c.color() for c in Cells];
	ucolor=set().union(*colors);
	return Cell(area,ucolor);

# mother cells contain all there neighboors	
def mothers(cells,neighboorsmap):
	R=range(len(cells));
	mothers=set();
	for k in R:
		ismother=True;
		mothercolor=set(cells[k].color());
		if not k in neighboorsmap:
		#	print("zombie:",k);
			continue;
		for n in neighboorsmap[k]:
			neighboor=cells[n];
			color_neighboor=set(neighboor.color())
			if not color_neighboor.issubset(mothercolor):
				ismother=False;
				break;
		if ismother:
			mothers.add(k);
	return mothers;

class ColorAccumulator:
	def __init__(self,C):
		self._container = list();
		self.C=C;
		
	def __call__(self, mother_set):
		self._container.append(mother_set);

	def container(self):
		return self._container;	

# a (sub)mother is a union of cells.
visited=set();
def walk(cells,mother_set,neighboorsmap,functor):
	global visited;	
	if type(mother_set) is type(0):
		mother_set={mother_set};
		visited=set();

	# gard against expn explosion	
	if tuple(mother_set) in visited:
		return;
	visited.add(tuple(mother_set));
	
	C=color([cells[k] for k in mother_set]);
	if len(C)<=1:
		print("max depth:",len(mother_set)," (empty colors)")	
		return;
	if len(mother_set)>8:
		#print("depth:",len(mother_set)," color", C)
		#print("max recursion depth reached");
		return;
	functor(mother_set);
	depth=len(mother_set);
	neighboors=set().union(*[neighboorsmap[k] for k in mother_set])-mother_set;
	if not neighboors:
		print("max depth:",len(mother_set)," color", C," (no neighboors)")
		return;	
	for n in neighboors:
		submother_set=mother_set.union({n});
		# speed up: premature filter
		C=color([cells[k] for k in submother_set]);
		if len(C)<=1:
			continue;
		walk(cells,submother_set,neighboorsmap,functor);
		

# now the question is: how many cell do we have, which
# (1) are not mothers, and
# (2) have no mother.
# That is the question.

def zombies(cells,mothers,neighboors):
	return None;	
	
if __name__ == '__main__':
	sys.exit(main())  

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

# now the question is: how many cell do we have, which
# (1) are not mothers, and
# (2) have no mother.
# That is the question.

def zombies(cells,mothers,neighboors):
	return None;	
	
if __name__ == '__main__':
	sys.exit(main())  

#!/usr/bin/env python3

import sys
import math;
import coloredmap;

def boxwidth():
	return 50

def index(u):
	nx=math.floor(u.x()/boxwidth());
	my=math.floor(u.y()/boxwidth());
	return (nx,my);

def cell_lookup(cells,indexes,point):
	ind=index(point);
	for k in indexes:
		if ind in cells[k].area():
			return k;
	return None;	

def cells_with_color(cells,color):
	ret=list();	
	for k in range(len(cells)):
		if color in cells[k].color():
			ret.append(k);
	return ret;

def neighboor_along_points(mapping,cells,traces,tk):
	points=traces[tk].geometry();
	cell=dict();
	for k in range(len(points)):
		cell[k]=cell_lookup(cells,cells_with_color(cells,tk),points[k]);
		if k==0:
			continue;
		i0=cell[k];
		i1=cell[k-1]
		if i0 is None or i1 is None:
			continue;
		if i0 not in mapping:
			mapping[i0]=set();
		if i1 not in mapping:
			mapping[i1]=set();
		mapping[i0].add(i1);
		mapping[i1].add(i0);
	return False;		
	
# big map		
def neighboorsmap(cells,traces):
	ret=dict();
	R=range(len(cells));
	for k in range(len(traces)):
		print("along",k);
		neighboor_along_points(ret,cells,traces,k)
	for k in ret:
		s=ret[k];
		if k in s:
			s.remove(k);
		print(k,"->",ret[k])		
	return ret;			


if __name__ == '__main__':
	sys.exit(main())  

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

def cell_lookup_index(cells,indexes,point):
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
	if not ret:
		# due to cell cleanup
		pass;
	return ret;

def cell_lookup_color(cells,color,point):
	I=cells_with_color(cells,color);
	assert(I);
	return cell_lookup_index(cells,I,point);

def neighboor_along_points(mapping,cells,traces,tk):
	points=traces[tk].geometry();
	cell=dict();
	index=cells_with_color(cells,tk);
	for k in range(len(points)):
		cell[k]=cell_lookup_index(cells,index,points[k]);
		if k==0:
			continue;
		# from i1 to i0	
		i0=cell[k];
		i1=cell[k-1]
		
		# out of cells
		if i0 is None or i1 is None:
			continue;
		
		# init
		if i0 not in mapping:
			mapping[i0]=set();
		if i1 not in mapping:
			mapping[i1]=set();
			
		mapping[i1].add(i0);
		mapping[i0].add(i1);
	return False;		
	
# big map		
def neighboorsmap(cells,traces):
	ret=dict();
	R=range(len(cells));
	for k in range(len(traces)):
		neighboor_along_points(ret,cells,traces,k)
	for k in ret:
		s=ret[k];
		if k in s:
			s.remove(k);
		#print(k,"->",ret[k])
	neighboor_along_points(ret,cells,traces,10)
	return ret;

def simplify(cells,bigmap):
	ret=dict();
	R=range(len(cells));
	for k in cells:
		if len(c.area())<10:
			# we want to remove k
			N=bigmap[k];
			for n1 in N:
				for n2 in N:
					ret[n1].add(n2)
			bigmap.remove(k);
	return ret;	


if __name__ == '__main__':
	sys.exit(main())  

#!/usr/bin/env python3

import sys
import math;
from . import coloredmap;

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
	ret=set();	
	for k in range(len(cells)):
		if color in cells[k].color():
			ret.add(k);
	if not ret:
		# due to cell cleanup
		pass;
	return ret;

def cell_lookup_color(cells,color,point):
	I=cells_with_color(cells,color);
	assert(I);
	return cell_lookup_index(cells,I,point);

if __name__ == '__main__':
	sys.exit(main())  

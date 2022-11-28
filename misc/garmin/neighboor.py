#!/usr/bin/env python3

import sys
import math;

def boxwidth():
	return 50

def index(u):
	nx=math.floor(u.x()/boxwidth());
	my=math.floor(u.y()/boxwidth());
	return (nx,my);

def subtracks(indexset,points):
	ret=list();
	first=None;
	last=None;
	for k in range(len(points)):
		inside=index(points[k]) in indexset;
		if not first and inside:
			first=k;
		if (not inside and first) or k==len(points)-1:
			ret.append((first,last));
			first=None;
			last=None;
		last=k;
	return ret;

def neighboor_with_points_d(cell1,cell2,points):
	S1=subtracks(cell1.area(),points);
	S2=subtracks(cell2.area(),points);
	for s1 in S1:
		(first1,last1)=s1;
		for s2 in S2:
			(first2,last2)=s2;
			d1=abs(first1-last2);
			d2=abs(first2-last1);
			assert(d1>=1 and d2>=1);
			if d1==1 or d2==1:
				return True;
	return False;

def neighboor_with_points(cell1,cell2,points):
	I1=dict();
	I2=dict();
	for k in range(len(points)):
		I1[k]=index(points[k]) in cell1.area();
		I2[k]=index(points[k]) in cell2.area();
		if k==0:
			continue;
		# point cannot be in 2 cells
		assert(not (I1[k] and I2[k]));
		# point moves from one cell to another
		toI2=I1[k-1] and I2[k];
		toI1=I2[k-1] and I1[k];
		if toI1 or toI2:
			return True;
	return False;		
	
def neighboor(cell1,cell2,traces):
	color1=set(cell1.color());
	color2=set(cell1.color());
	common=color1.intersection(color2)
	if not common:
		return False;
	for k in common:
		if neighboor_with_points(cell1,cell2,traces[k].geometry()):
			return True;
	return False;

# big map		
def neighboorsmap(cells,traces):
	ret=dict();
	R=range(len(cells));
	for k in R:
		ret[k]=set();
		for l in R:
			if l >= k:
				continue;
			if neighboor(cells[k],cells[l],traces):
				ret[k].add(l);
				assert(l in ret);
				ret[l].add(k);
		print(k,"->",ret[k])		
	return ret;			


if __name__ == '__main__':
	sys.exit(main())  

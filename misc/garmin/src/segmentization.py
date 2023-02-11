#!/usr/bin/env python3

import sys
import copy;
import math;

def neighbours(a):
	(n,m)=a;
	R=set();
	for k in [-1,0,+1]:
		for l in [-1,0,+1]:
			R.add((n+k,m+l));
	return R;

def segments(indexset):
	R=list();
	A=copy.deepcopy(indexset);
	C=set();
	while A:
		Q=None;
		for a in A:
			Q=[a];
			break;
		if not Q:
			return;
		assert(Q);
		while Q:
			a=Q.pop(0);
			for n in A.intersection(neighbours(a)):
				if not n in C:
					C.add(n); # label
					Q.append(n); # add to the queue
		R.append(copy.deepcopy(C));
		A=A-C;
		C.clear();
	RB=list();	
	return R;


def containspoint(area,u):
	nx=math.floor(u.x()/50);
	my=math.floor(u.y()/50);
	return (nx,my) in area;

def parts(area,P):
	first=None;
	last=None;
	rets=list();
	inside=dict();
	for k in range(len(P)):
		p=P[k];	
		inside[k]=containspoint(area,P[k]);
		if k==0:
			continue;	
		if not first and inside[k]:
			first=k;
		if first and (not inside[k] or k==len(P)-1):
			last=k;	
			rets.append([first,last+1]);
			first=None;
			last=None;
	for r in rets:
		[a,b]=r;
		print(a,"->",b);
	return rets;	

if __name__ == '__main__':
	sys.exit(main())  

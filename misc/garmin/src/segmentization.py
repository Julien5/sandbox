#!/usr/bin/env python3

import sys
import copy;
import math;
import copy;

def neighbours(a):
	(n,m)=a;
	R=set();
	for k in [-1,0,+1]:
		for l in [-1,0,+1]:
			R.add((n+k,m+l));
	return R;

def subgroups(indexset):
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

def denoise(I):
	assert(I);
	P=copy.deepcopy(I);
	clean=[P[0]];
	# remove small gaps:
	for k in range(len(P)):
		if k==0:
			continue;
		[a0,b0]=P[k-1];
		[a1,b1]=P[k];
		assert(a1>=b0);
		d=a1-b0;
		if d<10:
			clean[-1][1]=b1;
		else:
			clean.append(P[k]);
	# remove small section
	ret=list();
	for k in range(len(clean)):
		[a,b]=clean[k];
		#if b-a>20:
		ret.append(clean[k])
	return ret;		

def parts(area,P):
	first=None;
	last=None;
	rets=list();
	inside=dict();
	for k in range(len(P)):
		p=P[k];	
		inside[k]=containspoint(area,P[k]);
		if not first and inside[k]:
			first=k;
		if first and (not inside[k] or k==len(P)-1):
			last=k;
			#print(first,"-->",last);
			rets.append([first,last]);
			first=None;
			last=None;
	rets2=denoise(rets)
	R=rets2;
	#print("--")
	#for r in R:
	#	[a,b]=r;
	#	print(a,"->",b);
	return R;	

if __name__ == '__main__':
	sys.exit(main())  

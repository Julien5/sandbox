#!/usr/bin/env python3

import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__),".."));

import elevation;
from richwaypoint import RichWaypoint;

def argmax(x,R):
	return max(R, key=lambda i: x[i])

def RK(x,start,K):
	return [k for k in range(start,len(x)) if abs(x[start]-x[k])<K];

def R100(x,start):
	return RK(x,start,100);

def R20(x,start):
	return RK(x,start,20);

def summits(x,y):
	start=0;
	ret=list();
	while start<len(x):
		R=R20(x,start);
		kmax=argmax(y,R);
		start=max(R)+1;
		ret.append(kmax);
	return ret;

def waypoints_summits(P):
	ret=dict();
	x,y=elevation.load(P);
	assert(len(x)==len(y));
	summit_indices=summits(x,y);
	assert(not 0 in summit_indices);
	assert(len(summit_indices)>=1);
	for n in range(len(summit_indices)):
		k=summit_indices[n];
		rw=RichWaypoint(P[k]);
		rw.distance=x[k]*1000;
		ret[rw.distance]=rw;
	return ret;


from rdp import rdp

#>>> rdp([[1, 1], [2, 2], [3, 3], [4, 4]])
#[[1, 1], [4, 4]]

def waypoints_douglas_worker(X):
	R=rdp(X,5);
	return [r[0] for r in R];

def decimate(y,indices):
	# we observe 3-consecutive subsets
	if len(y)<3:
		return y;
	I=sorted(list(indices));
	rindices={0};
	for k in range(1,len(I)-1):
		[a,b,c]=[y[I[i]] for i in range(k-1,k+2)];
		A=b>a;
		B=c>b;
		if A==B:
			continue;
		rindices.add(I[k]);
	rindices.add(I[-1]);
	return rindices;

def waypoints_douglas(P):
	ret=dict();
	x,y=elevation.load(P);
	assert(len(x)==len(y));
	X=[[x[k],y[k]] for k in range(len(x))];
	start=0;
	indices=set();
	while start<len(x):
		r100=R100(x,start);
		begin=r100[0];
		end=r100[-1];
		D=waypoints_douglas_worker(X[begin:end]);
		for d in D:
			indices.add(x.index(d));
		start=end+1;	
	ret=dict();
	rindices=indices;
	while True:
		before=len(rindices);
		rindices=decimate(y,rindices);
		after=len(rindices);
		if before==after:
			break;
	print("removed",len(indices)-len(rindices),"points")
	for k in rindices:
		rw=RichWaypoint(P[k]);
		rw.distance=x[k]*1000;
		ret[rw.distance]=rw;
	return ret;

def waypoints(P):
	#S=waypoints_summits(P);
	D=waypoints_douglas(P);
	return D;

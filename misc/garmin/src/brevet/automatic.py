#!/usr/bin/env python3

import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__),".."));

import elevation;
from richwaypoint import RichWaypoint;

def argmax(x,R):
	return max(R, key=lambda i: x[i])

def R20(x,start):
	return [k for k in range(start,len(x)) if abs(x[start]-x[k])<20];

def summits(x,y):
	start=0;
	ret=list();
	while start<len(x):
		R=R20(x,start);
		kmax=argmax(y,R);
		start=max(R)+1;
		ret.append(kmax);
	return ret;

def waypoints(P,start):
	ret=dict();
	x,y=elevation.load(P);
	summit_indices=summits(x,y);
	assert(not 0 in summit_indices);
	assert(len(summit_indices)>=1);
	for n in range(len(summit_indices)):
		k=summit_indices[n];
		rw=RichWaypoint(P[k]);
		rw.distance=x[k]*1000;
		ret[rw.distance]=rw;
	return ret;

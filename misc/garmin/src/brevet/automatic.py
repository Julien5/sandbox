#!/usr/bin/env python3

import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__),".."));

import elevation;
from richwaypoint import RichWaypoint;

def argmax(x,R):
	return max(R, key=lambda i: x[i])

def argmin(x,R):
	return min(R, key=lambda i: x[i])

def RK(x,start,K):
	if K<0:
		return [k for k in range(0,start) if abs(x[start]-x[k])<abs(K)];
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

def fixup_extremas(x,y,indices):
	I=sorted(list(indices));
	rindices={0};
	ret={I[0]};
	for k in range(1,len(I)-1):
		xprev=I[k-1];
		xk=I[k];
		domain=RK(x,xk,-3) + RK(x,xk,3);
		assert(xk in domain);
		if y[xprev] > y[xk]: # descending
			xm=argmin(y,domain);
			assert(y[xm]<=y[xk]);
		else:
			xm=argmax(y,domain);
			assert(y[xm]>=y[xk]);
		assert(xm in domain);	
		print(f"{x[xk]:3.1f}->{x[xm]:3.1f} -> delta={x[xk]-x[xm]:3.1f}");	
		ret.add(xm);	
	ret.add(I[-1]);
	return ret;

def decimate_flat_3(x,y,indices):
	# we observe 3-consecutive subsets
	if len(y)<3:
		return y;
	I=sorted(list(indices));
	ret={0};
	k=1;
	while k<len(I)-1:
		[ya,yb,yc]=[y[I[i]] for i in range(k-1,k+2)];
		[xa,xb,xc]=[x[I[i]] for i in range(k-1,k+2)];
		dy=max([ya,yb,yc])-min([ya,yb,yc]);
		assert(dy>=0);
		dx=xc-xa;
		slope=dy/(dx*10); # dx is in kilometer
		if (slope<1 and dx<=17): # it is flat
			ret.add(I[k+1]);
			k=k+2;
		else:
			ret.add(I[k]);
			k=k+1;
	ret.add(I[-1]);
	return ret;

def decimate_flat(x,y,indices):
	return decimate_flat_3(x,y,indices);

def decimate_same_slope(x,y,indices):
	# we observe 3-consecutive subsets
	if len(y)<3:
		return y;
	I=sorted(list(indices));
	rindices={0};
	for k in range(1,len(I)-1):
		[ya,yb,yc]=[y[I[i]] for i in range(k-1,k+2)];
		sA=yb>ya;
		sB=yc>yb;
		if sA==sB:
			continue;
		rindices.add(I[k]);
	rindices.add(I[-1]);
	return rindices;


def decimate(x,y,indices):
	r=decimate_same_slope(x,y,indices);
	r=decimate_flat(x,y,r);
	return r;


def waypoints_douglas(P):
	ret=dict();
	x,y=elevation.load(P);
	assert(len(x)==len(y));
	X=[[x[k],y[k]] for k in range(len(x))];
	start=0;
	indices=set();
	
	r100=R100(x,start);
	begin=0;
	end=len(X)
	D=waypoints_douglas_worker(X[begin:end]);
	for d in D:
		indices.add(x.index(d));
	ret=dict();
	rindices=indices;
	while True:
		before=len(rindices);
		print(f"*** decimate [{len(rindices):3d}]***");
		rindices=decimate_same_slope(x,y,rindices);
		after=len(rindices);
		if before==after:
			break;
	while True:
		before=len(rindices);
		print(f"*** decimate flat [{len(rindices):3d}]***");
		rindices=decimate_flat(x,y,rindices);
		after=len(rindices);
		if before==after:
			break;
	rindices=fixup_extremas(x,y,rindices);
	print("removed",len(indices)-len(rindices),"points")
	for k in sorted(rindices):
		rw=RichWaypoint(P[k]);
		rw.distance=x[k]*1000;
		print(f"waypoint at {x[k]:3.0f}");
		ret[rw.distance]=rw;
	return ret;

def waypoints(P):
	#S=waypoints_summits(P);
	D=waypoints_douglas(P);
	return D;

def slope(x,y,p1,p2):
	cumulative_x=0;
	cumulative_y=0;
	start=p1.distance;
	end=p2.distance;
	for k in range(len(x)):
		d=x[k]*1000;
		if d<start:
			continue;
		if d>end:
			break;
		if y[k]>y[k-1] and k>0:
			cumulative_y+=y[k]-y[k-1];
			assert(x[k]>x[k-1]);
			cumulative_x+=1000*(x[k]-x[k-1]);
	return cumulative_x,cumulative_y;


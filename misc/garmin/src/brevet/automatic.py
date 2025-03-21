#!/usr/bin/env python3

import sys
import os
import math

sys.path.append(os.path.join(os.path.dirname(__file__),".."));

import elevation;
from richwaypoint import RichWaypoint;

def argmax(x,R):
	return max(R, key=lambda i: x[i])

def argmin(x,R):
	return min(R, key=lambda i: x[i])

def RK(x,start,km):
	m=1000*km;
	if m<0:
		return [k for k in range(0,start) if abs(x[start]-x[k])<abs(m)];
	return [k for k in range(start,len(x)) if abs(x[start]-x[k])<m];

def R100(x,start):
	return RK(x,start,100);

def R20(x,start):
	return RK(x,start,20);

from rdp import rdp

#>>> rdp([[1, 1], [2, 2], [3, 3], [4, 4]])
#[[1, 1], [4, 4]]
def waypoints_douglas_worker(X):
	R=rdp(X,100);
	return [r[0] for r in R];

def fixup_extremas(x,y,indices):
	I=indices;
	rindices=[0];
	ret=[I[0]];
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
		#print(f"{x[xk]:3.1f}->{x[xm]:3.1f} -> delta={x[xk]-x[xm]:3.1f}");	
		ret.append(xm);	
	ret.append(I[-1]);
	return ret;

def decimate_flat_3(x,y,indices):
	# we observe 3-consecutive subsets
	if len(y)<3:
		return y;
	I=indices;
	ret=[0];
	k=1;
	while k<len(I)-1:
		[ya,yb,yc]=[y[I[i]] for i in range(k-1,k+2)];
		[xa,xb,xc]=[x[I[i]] for i in range(k-1,k+2)];
		dy=max([ya,yb,yc])-min([ya,yb,yc]);
		assert(dy>=0);
		dx=xc-xa;
		slope=dy/(dx/100); 
		if (slope<1 and dx<=17): # it is flat
			ret.append(I[k+1]);
			k=k+2;
		else:
			ret.append(I[k]);
			k=k+1;
	ret.append(I[-1]);
	return ret;

def decimate_flat(x,y,indices):
	return decimate_flat_3(x,y,indices);

def decimate_same_slope(x,y,indices):
	# we observe 3-consecutive subsets
	if len(y)<3:
		return y;
	I=indices;
	rindices=[0];
	for k in range(1,len(I)-1):
		[ya,yb,yc]=[y[I[i]] for i in range(k-1,k+2)];
		sA=yb>ya;
		sB=yc>yb;
		if sA==sB:
			continue;
		rindices.append(I[k]);
	rindices.append(I[-1]);
	return rindices;


def decimate(x,y,indices):
	r=decimate_same_slope(x,y,indices);
	r=decimate_flat(x,y,r);
	return r;

def generate_point_at_index(index,P,d):
	rw=RichWaypoint(P[index]);
	rw.distance=d[index];
	rw.index=index;
	rw.type="A";
	return rw;

def waypoints_douglas(P,E):
	(x,y)=E.xy();
	assert(len(x)==len(y));
	X=[[x[k],y[k]] for k in range(len(x))];
	start=0;
	indices=list();
	
	r100=R100(x,start);
	begin=0;
	end=len(X)
	D=waypoints_douglas_worker(X[begin:end]);
	for d in D:
		indices.append(x.index(d));
	assert(sorted(indices)==indices);

	rindices=indices;
	while True:
		before=len(rindices);
		#print(f"*** decimate [{len(rindices):3d}]***");
		rindices=decimate_same_slope(x,y,rindices);
		after=len(rindices);
		if before==after:
			break;
	while True:
		before=len(rindices);
		#print(f"*** decimate flat [{len(rindices):3d}]***");
		rindices=decimate_flat(x,y,rindices);
		after=len(rindices);
		if before==after:
			break;
	rindices=fixup_extremas(x,y,rindices);
	#print("removed",len(indices)-len(rindices),"points")
	assert(rindices==sorted(rindices));
	ret=list();
	assert(sorted(rindices)==rindices);
	for k in rindices:
		rw=generate_point_at_index(k,P,x);
		ret.append(rw);
	return ret;


def generate_point_at_distance(distance,P,d):
	index=0;
	for k in range(len(d)):
		index=k;
		if d[k]>distance:
			break;
	return generate_point_at_index(index,P,d);

def nextd(distance,delta):
	return (math.floor(distance/delta)+1)*delta;


def subsample(d_begin,d_end,delta,P,d):
	ret=list();
	while True:
		#d_begin=nextd(d_begin,delta);
		d_begin=d_begin+delta;
		if d_end - d_begin < 5000:
			break;
		a=generate_point_at_distance(d_begin,P,d);
		ret.append(a);
	return ret;

def waypoints_subsample(P,E,D):
	delta=15000;
	A=list();
	(d,y)=E.xy();
	for k in range(len(D)):
		if k==0:
			continue;
		wprev=D[k-1];
		w=D[k];
		if w.distance-wprev.distance>delta:
			Aloc=subsample(wprev.distance,w.distance,delta,P,d);
			A.extend(Aloc);
	return A;		

def waypoints(P,E):
	D=waypoints_douglas(P,E);
	A=waypoints_subsample(P,E,D);
	R=D+A;
	return sorted(R, key=lambda w: w.distance);

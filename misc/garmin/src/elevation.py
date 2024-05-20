#!/usr/bin/env python3

import os;
import sys;

import matplotlib.pyplot as plt
import numpy as np

import readtracks;
import utils;

def readfile(filename):
	name,R=readtracks.readpoints(filename);
	return R;

def load(P):
	x=[];
	y=[];
	pprev=None;
	for k in range(len(P)):
		# print(p.latitude,p.longitude,p.elevation);
		if k==0:
			x.append(0);
		else:
			x.append(x[k-1]+utils.distance(P[k-1],P[k]));
		y.append(P[k].elevation);
	return x,y;	
		
def plot(x,y):
	fig, axs = plt.subplots(1, 1, layout='constrained')
	x,y=load();
	axs.plot(x, y)
	# axs.plot(t, s1)
	axs.set_xlim(0, x[-1])
	axs.set_xlabel('t')
	axs.set_ylabel('s1')
	axs.grid(True)
	plt.show();

from rdp import rdp

def round(x):
	return int(x);

def douglas_peucker(x,y,epsilon):
	xm=[round(1000*x[k]) for k in range(len(x))];
	Xi=[[xm[k],y[k]] for k in range(len(x))];
	R=rdp(Xi,epsilon);
	indices=list();
	for r in R:
		indices.append(xm.index(r[0]));
	return indices;

def compute(x,y,epsilon=0):
	indices=douglas_peucker(x,y,epsilon);
	ret=0;
	for i in range(len(indices)):
		if i==0:
			continue;
		k=indices[i];
		kprev=indices[i-1];
		if y[k]>y[kprev]:
			ret+=(y[k]-y[kprev])
	return ret;

class Elevation:
	def __init__(self,track):
		x=list();
		y=list();
		for k in range(len(track)):
			P=track[k];
			if k==0:
				x.append(0);
			else:
				Pprev=track[k-1];
				x.append(x[k-1]+utils.distance(Pprev,P));
			y.append(P.elevation);
		self._xy=(x,y);
		self.estimate_positive_elevation();

	def estimate_positive_elevation(self,windices=None):
		(x,y)=self.xy();
		dindices=douglas_peucker(x,y,10);
		assert(len(dindices)==len(set(dindices)));
		assert(0 in dindices and len(x)-1 in dindices);
		indices=dindices;
		if not windices is None:
			indices=dindices+windices;
		indices=sorted(set(indices));
		assert(len(indices)>=len(dindices));
		self._positive_elevation=list();
		dy=0;
		dx=0;
		self._positive_elevation=dict();
		for i in range(len(indices)):
			if i==0:
				self._positive_elevation[0]=(0,dx,dy);
				continue;
			kbegin=indices[i-1];
			kend=indices[i];
			if y[kend] > y[kbegin]:
				dy += y[kend]-y[kbegin];
				dx += x[kend]-x[kbegin];
			self._positive_elevation[kend]=(x[kend],dx,dy)

	def floor_index(self,index):
		I=sorted(self._positive_elevation.keys());
		for i in I:
			if i>=index:
				return i;
		assert(False);	
		return None;	
		
	def elevation_from_to(self,index1,index2):
		assert(index1 in self._positive_elevation);
		assert(index2 in self._positive_elevation);
		xe1=self._positive_elevation[index1];
		xe2=self._positive_elevation[index2];
		dx=xe2[1]-xe1[1]
		dy=xe2[2]-xe1[2];
		if xe1 and xe2:
			return dx,dy;
		return None,None;

	def xy(self):
		return self._xy;

	def slope(self,begin,end):
		return self.elevation_from_to(begin,end);

	def y_at(self,index):
		return self._xy[1][index];

	def positiv_elevation_at(self,index):
		return self._positive_elevation[index][2];

def print_elevation(e,prefix=None):
	if prefix:
		print(f"{prefix:s} ",end="")
	print(f"elevation={e:5.1f}");

def main(filename,epsilon=0):
	track=readfile(filename);
	E=Elevation(track);
	for k in range(len(track)):
		print(k,E.elevation_from_to(0,k));
	return;
	x,y=load(readfile(filename));
	# plot(x,y);
	elevation=compute(x,y,epsilon);
	Elevation();
	print_elevation(elevation,"computed");

def find_test_file(filename):
	return os.path.join(os.path.dirname(__file__),"..","test/",filename);
	
if __name__ == "__main__":
	# test/elevation.gpx,
	# gpx.studio: 1257.
	# komoot:     1390
	main(find_test_file("elevation-oa.gpx"),epsilon=5);
	sys.exit();
		
	epsilon=1
	print("*** OA");
	for epsilon in [0, 0.01, 0.05, 0.1, 1, 5]:
		print("epsilon=",epsilon);
		main(find_test_file("elevation-oa.gpx"),epsilon);
	print();
	
	print("*** studio");
	for epsilon in [0, 0.01, 0.05, 0.1, 1, 5]:
		print("epsilon=",epsilon);
		main(find_test_file("elevation.gpx"),epsilon);
	print();
	
	print("komoot");
	for epsilon in [0, 0.01, 0.05, 0.1, 1, 5]:
		print("epsilon=",epsilon);
		main(find_test_file("elevation-komoot.gpx"),epsilon);
	print("done");


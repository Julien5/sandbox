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
	print("#index",len(indices));
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
		indices=douglas_peucker(x,y,50);
		self._positive_elevation=list();
		for i in range(len(indices)):
			if i==0:
				continue;
			k=indices[i];
			kprev=indices[i-1];
			distance=x[k];
			elevation=y[k];
			if y[k] > y[kprev]:
				elevation += y[k]-y[kprev];
			for k in range(kprev+1,k+1):
				self._positive_elevation.append((x[k],elevation));
		last_k=len(self._positive_elevation)-1;
		last_value=self._positive_elevation[-1];
		for k in range(last_k+1,len(track)):
			self._positive_elevation.append(last_value);
		assert(len(self._positive_elevation)==len(track));
			
	def elevation_from_to(self,index1,index2):
		xe1=next(filter(lambda xe: xe[0]==index1+1, self._positive_elevation));
		xe2=next(filter(lambda xe: xe[0]==index2, self._positive_elevation));
		if xe1 and xe2:
			return xe2[1]-xe1[1];
		return None;

	def xy(self):
		return self._xy;

	def slope(self,start,end):
		cumulative_x=0;
		cumulative_y=0;
		(x,y)=self.xy();
		for k in range(len(x)):
			d=x[k];
			if d<start:
				continue;
			if d>end:
				break;
			if y[k]>y[k-1] and k>0:
				cumulative_y+=y[k]-y[k-1];
				assert(x[k]>x[k-1]);
				cumulative_x+=(x[k]-x[k-1]);
		return cumulative_x,cumulative_y;

def print_elevation(e,prefix=None):
	if prefix:
		print(f"{prefix:s} ",end="")
	print(f"elevation={e:5.1f}");

def main(filename,epsilon=0):
	x,y=load(readfile(filename));
	# plot(x,y);
	elevation=compute(x,y,epsilon);
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


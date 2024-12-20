#!/usr/bin/env python3

import os;
import sys;

import matplotlib.pyplot as plt
import numpy as np

import readtracks;
import utils;

def readfile(filename):
	name,R,creator=readtracks.readpoints(filename);
	return R;

def plot(x,y):
	fig, axs = plt.subplots(1, 1, layout='constrained')
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
	Xi=[[x[k],y[k]] for k in range(len(x))];
	R=rdp(Xi,epsilon);
	# Some ill-formed gpx track have the form
	# (p1,e1) (p2,e2) (p3,e3) (p3,e3') (p4,e4) ...
	# The same point is repeated with another elevation.
	# This causes repetitions in the indices of DP output.
	# => indices is a set, not a list.
	indices=set();
	last_index=0;
	for r in R:
		index=x.index(r[0],last_index);
		indices.add(index);
		last_index=index;
	return sorted(indices);

import srtm
class Elevation:
	def __init__(self,track,douglas_peucker_epsilon=2):
		x=list();
		y=list();
		elevation_data = srtm.get_data()
		for k in range(len(track)):
			P=track[k];
			if k==0:
				x.append(0);
			else:
				Pprev=track[k-1];
				d=utils.distance(Pprev,P);
				x.append(x[k-1]+d);
				
			if not P.elevation is None:
				y.append(P.elevation);
			else:	
				elevation=elevation_data.get_elevation(P.latitude,P.longitude);
				y.append(elevation);
		assert(len(x)==len(y));
		assert(len(x)==len(track));
		self._xy=(x,y);
		self.douglas_peucker_epsilon=douglas_peucker_epsilon;

	def setCreator(self,creator):
		# adjust elevation filtering to the source
		# OA -> epsilon=16
		# komoot and gpx.studio -> epsilon=2
		self.douglas_peucker_epsilon=20;
		if "outdooractive" in creator:
			self.douglas_peucker_epsilon=25;
		print(f"using epsilon={self.douglas_peucker_epsilon:3.0f} for creator={creator:s}");	

	def estimate_positive_elevation(self,windices=None):
		(x,y)=self.xy();
		dindices=douglas_peucker(x,y,self.douglas_peucker_epsilon);
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
		for n in range(len(I)):
			if (n+1)==len(I): # the next is past end
				return I[-1];
			if I[n+1]>=index:
				return I[n];
		assert(False);	
		return None;

	def floor_distance(self,dist):
		x,y=self.xy();
		ret=0;
		for k in range(len(x)):
			if x[k]<dist:
				ret=k;
			else:
				break;
		return ret;	
		
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

def main():
	filename="/home/julien/tours/self/2024/05/gpx/studio/foret-noire.gpx";
	track=readfile(filename);
	E=Elevation(track,20);
	E.estimate_positive_elevation();
	x,y=E.xy()
	begin=E.floor_index(E.floor_distance(33100));
	end=E.floor_index(E.floor_distance(38000));
	print(E.elevation_from_to(begin,end));
	plot(x,y);
	
def maind(filename,epsilon):
	track=readfile(filename);
	E=Elevation(track,epsilon);
	E.estimate_positive_elevation();
	dx,dy=E.elevation_from_to(0,len(track)-1);
	print(filename,dy);

def find_test_file(filename):
	return os.path.join(os.path.dirname(__file__),"..","test/",filename);
	
if __name__ == "__main__":
	# test/elevation.gpx,
	# gpx.studio: 1257.
	# komoot:     1390
	#main(find_test_file("elevation-oa.gpx"),16);
	#main(find_test_file("elevation-studio.gpx"),2);
	#main(find_test_file("elevation-komoot.gpx"),2);
	main();
	print("done");


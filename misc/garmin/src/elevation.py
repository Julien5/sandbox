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
			# todo: not divide by 1000.
			x.append(x[k-1]+utils.distance(P[k-1],P[k])/1000);
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


#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

import readtracks;
import utils;

def readfile(filename):
	return readtracks.readpoints(filename);

def load(P):
	x=[];
	y=[];
	pprev=None;
	for k in range(len(P)):
		# print(p.latitude,p.longitude,p.elevation);
		if k==0:
			x.append(0);
		else:
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

def compute(x,y):
	ret=0;
	for k in range(len(y)):
		if k==0:
			continue;
		if y[k]>y[k-1]:
			ret+=(y[k]-y[k-1])
	return ret;

def print_elevation(e,prefix=None):
	if prefix:
		print(f"{prefix:s} ",end="")
	print(f"elevation={e:5.1f}");

def main(filename):
	print("loading",filename);
	x,y=load(readfile(filename));
	# plot(x,y);
	elevation=compute(x,y);
	print_elevation(elevation,"computed");

if __name__ == "__main__":
	# test/elevation.gpx,
	# gpx.studio: 1257.
	# komoot:     1390
	main("test/elevation.gpx");
	print_elevation(1257,"expected");

	print();
	main("test/elevation-komoot.gpx");
	print_elevation(1390,"expected");

	print();
	main("test/elevation-oa.gpx");
	print_elevation(1099,"expected");
	
	print("done");


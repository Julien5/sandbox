#!/usr/bin/env python3

import matplotlib.pyplot as plt
import csv
import time
import sys;

def load(logfile):
	x = []
	y = []
	with open(logfile,'r') as csvfile:
		plots = csv.reader(csvfile, delimiter=':')
		for row in plots:
			if len(row) <= 2:
				continue;
			t=int(row[0]);
			x.append(t);
			v=int(row[1]);
			# print(v);
			y.append(v);
	if len(y)<=100:
		return None,None;
	x=x[-100:]
	y=y[-100:]
	return x,y;

graph=None;

def plot(x,y):
	global graph;
	if graph is None:
		print("create graph");
		graph=plt.plot(x,y, label='data')[0];
		plt.xlabel('x')
		plt.ylabel('y')
		plt.title('data')
		plt.legend();
	plt.axis([min(x), max(x), 0, 120]);
	graph.set_xdata(x);
	graph.set_ydata(y);
	plt.draw();
	plt.pause(0.01);

def main():
	logfile=sys.argv[1];
	while True:
		print("replot",logfile);
		t0=time.time();
		x,y=load(logfile);
		print(time.time()-t0);
		if x and y:
			plot(x,y);
		time.sleep(0.1);


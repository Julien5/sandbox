#!/usr/bin/env python3

import matplotlib.pyplot as plt
import csv
import time

def load():
	x = []
	y = []
	with open('screenlog.0','r') as csvfile:
		plots = csv.reader(csvfile, delimiter=':')
		for row in plots:
			if len(row) <= 2:
				continue;
			x.append(len(x))
			v=int(row[2]);
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
		graph=plt.plot(x,y, label='data')[0];
	plt.xlabel('x')
	plt.ylabel('y')
	plt.title('data')
	plt.legend();
	plt.axis([min(x), max(x), 0,120]);
	graph.set_xdata(x);
	graph.set_ydata(y);
	plt.draw();
	plt.pause(0.01);

def main():
	while True:
		print("replot");
		x,y=load();
		if x and y:
			plot(x,y);
		time.sleep(0.1);

if __name__ == "__main__":
	main();

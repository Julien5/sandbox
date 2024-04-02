#!/usr/bin/env python3

import serial;
import matplotlib.pyplot as plt
import statistics;

graph=None;
N=100;
x=[0]*N;
y=[0]*N;


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
	plt.pause(0.00000000001);

def main():
	global x,y,N;
	port="/dev/ttyACM1"
	M=5;
	xk=[];
	yk=[];
	with serial.Serial(port, 9600) as ser:
		while True:
			line = ser.readline()   # read a '\n' terminated line
			data=line.decode("ascii").split();
			if not data:
				print("no data");
				continue;
			print(data,len(data));
			P=data[0].split(":");
			if len(P)<2:
				print("err",line);
				continue;
			try:
				xk.append(int(P[0]));
				yk.append(int(P[1]));
			except ValueError as e:
				print("error:",e);
				continue;
			if len(xk)<2:
				continue;
			x=x[1:len(x)];
			x.append(statistics.mean(xk));
			y=y[1:len(x)];
			y.append(statistics.mean(yk));
			xk=[];
			yk=[];
			M=M-1;
			if M==0:
				plot(x,y);
				M=15;

if __name__ == "__main__":
	main();

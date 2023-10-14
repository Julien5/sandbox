#!/usr/bin/env python3

import readgpx2;
import os;
import sys;
import datetime;
import math;
import output;

def readtours():
	#dir="test";
	dir="/home/julien/tracks/";
	newtracks=readgpx2.newtracksfromdir(dir);
	installed=readgpx2.install(newtracks);
	print("new:",len(installed));
	print("read all installed");
	T=readgpx2.read_all_installed();
	print("all:",len(T));
	C=dict();
	for t in T:
		if not t.category() in C:
			C[t.category()]=list();
		C[t.category()].append(t);
	print("OK");
	return C;

def main():
	C=readtours();
	for cat in C:
		print("#",cat);
		T=C[cat];
		if not T:
			continue;
		for t in sorted(T, key=lambda t: t.begintime()):
			output.print_stats(t);

if __name__ == '__main__':
	sys.exit(main())  

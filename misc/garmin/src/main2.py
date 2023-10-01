#!/usr/bin/env python3

import readgpx2;
import os;
import sys;
import datetime;
import math;
import output;

def readtours():
	dir="test";
	dir="/home/julien/tracks/";
	T=readgpx2.tracksfromdir(dir);
	assert(T);
	print("clean tracks..");
	T=readgpx2.install(T);

def main():
	readtours();

if __name__ == '__main__':
	sys.exit(main())  

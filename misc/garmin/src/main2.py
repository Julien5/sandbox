#!/usr/bin/env python3

import readgpx2;
import os;
import sys;
import datetime;
import math;
import output;

def readtours():
	T=readgpx2.tracksfromdir("test");
	assert(T);
	print("clean tracks..");
	T=readgpx2.install(T);

def main():
	readtours();

if __name__ == '__main__':
	sys.exit(main())  

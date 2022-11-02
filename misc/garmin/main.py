#!/usr/bin/env python3

import readgpx;
import sys;

def main():
	T=readgpx.tracks("testride.gpx");
	#for t in tracks:
	#	print(t.string());
	T=readgpx.tracksfromdir("/tmp/gpx/good");
	for t in T:
		P=t.points();
		date=list(P.keys())[-1]
		print(str(date),len(t.points()))
	
if __name__ == '__main__':
	sys.exit(main())  

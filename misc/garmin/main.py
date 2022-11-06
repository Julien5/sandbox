#!/usr/bin/env python3

import readgpx;
import sys;

def main():
	T=readgpx.tracks("testride.gpx");
	#for t in tracks:
	#	print(t.string());
	#T=readgpx.tracksfromdir("/home/julien/tracks/2022.10.01/GPX/");
	T=readgpx.tracksfromdir("test");
	for t in T:
		P=t.points();
		date=list(P.keys())[-1]
		stamp="none";
		if date:
			stamp=date.strftime("%d.%m-%H:%M:%S");
		duration=str(t.duration());
		print(f"{stamp:20s}{len(t.points()):4d} distance:{t.distance()/1000:8.1f} km duration:{duration:20s} {t.filename():40s}");
	
if __name__ == '__main__':
	sys.exit(main())  

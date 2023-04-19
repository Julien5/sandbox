#!/usr/bin/env python3

import gpxpy;
import gpxpy.gpx;
import readgpx;
import sys;

def main():
	gpx=readgpx.readgpx("/home/julien/brevets/300/oa.gpx");
	gpx_out = gpxpy.gpx.GPX();
	gpx_out.name="300";
	gpx_out.description="Brevet 300k";
	print(len(gpx.tracks));
	for T in gpx.tracks:
		Tout = gpxpy.gpx.GPXTrack();
		Tout.name = "300-processed"
		print("new track",len(gpx.tracks));
		for S in T.segments:
			if not S:
				print("skip.");
				continue;
			for w in readgpx.getOAWaypoints(S):
				gpx_out.waypoints.append(w);
			readgpx.cleansegment(S);
			S=readgpx.remove_time_and_elevation(S);
			Tout.segments.append(S);
		print("append");	
		gpx_out.tracks.append(Tout);
	readgpx.writegpx("/home/julien/brevets/300/processed.gpx",gpx_out);
		
if __name__ == '__main__':
	sys.exit(main())  

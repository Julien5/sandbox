#!/usr/bin/env python3

import readgpx;
import sys;
import datetime;
import track;
import geometry;
import projection;

def main():
	T=readgpx.tracks("/home/julien/brevets/200/track.gpx",order="index");
	# put the 4 seg together.
	out=track.Track("processed");
	for segment in T:
		print(len(segment.geometry()));
		out.append_subtrack(segment);

	G1=T[0].geometry();
	Go=out.geometry();
	for k in range(5):
		print(G1[k].string(),Go[k].string());

		
	print(len(out.geometry()));
	# start point= <trkpt lat="48.9817500" lon="10.9114220">
	latitude=48.9817500;
	longitude=10.9114220;
	startpoint=geometry.Point(latitude, longitude);
	(x,y)=projection.convert(latitude, longitude);
	startpoint=geometry.Point(x,y,latitude,longitude);

	k_start=-1;
	for k in range(len(Go)):
		p=Go[k];
		if p.latitude() == latitude and p.longitude() == longitude:
			assert(k_start==-1);
			k_start=k;

	print(k_start,p.string());
	t=datetime.datetime.now();
	delta=datetime.timedelta(seconds=1);
	out2=track.Track("processed");
	for k in range(len(Go)):
		j=k_start+k;
		out2.append(t,Go[j % len(Go)]);
		t=t+delta;
	readgpx.write(out2,"/home/julien/brevets/200/processed.gpx");	
		

		
if __name__ == '__main__':
	sys.exit(main())  

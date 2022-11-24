#!/usr/bin/env python3

import track;
import geometry;

import gpxpy
import gpxpy.gpx
import sys;
import os;
import xml.etree.cElementTree as mod_etree
import projection;

def cleansegment(segment):
	N=len(segment.points);
	p_prev=None
	for i in range(N):
		p=segment.points[i];
		for e in p.extensions:
			# this is what cause problems with gpsbabel
			if "outdooractive" in e.tag:
				e.tag="OA";
				

def readgpx(filename):
	try:
		gpx_file = open(filename, 'r');
		gpx = gpxpy.parse(gpx_file);
		return gpx;
	except Exception as e:
		print(filename);
		print(e)
		raise e;
		

def to_track(segment,filename):
	ret=track.Track(filename);
	for point in segment.points:
		latitude=point.latitude;
		longitude=point.longitude;
		# point.elevation
		time = point.time;
		(x,y)=projection.convert(latitude,longitude);
		ret.append(time,geometry.Point(x,y,latitude,longitude));
	return ret;	
		
def tracks(filename):
	gpx = readgpx(filename);
	ret = list();
	for T in gpx.tracks:
		for S in T.segments:
			cleansegment(S);
			ret.append(to_track(S,filename));
	return ret;

def tracksfromdir(dirname):
	ret=list();
	for root, dirs, files in os.walk(dirname):
		for file in files:
			if file.endswith(".gpx") and "Track_" in file:
				filename=os.path.join(root, file);
				try:	
					ret.extend(tracks(filename));
				except Exception as e:
					print("could not read:",filename);	
	return ret;

def clean(tracks):
	# first gather all distinct points	
	points=dict();
	for T in tracks:
		P=T.points();
		for p in P:
			points[p]=P[p]
	# as list
	times=sorted(list(points));
	R=list();
	T=None;
	N=len(times);
	ends=list();
	for k in range(N-1):
		t0=times[k];
		t1=times[k+1];
		d=(t1-t0).total_seconds();
		if d > 150:
			ends.append(k);
	kstart=0;
	kend=-1;
	while True:
		kstart=kend+1;
		if kstart >= len(times):
			break;	
		if ends: # not empty
			kend=ends.pop(0);
		else:
			kend=len(times);
		name=times[kstart].strftime("%d.%m[%H:%M]")
		T=track.Track(name);
		for i in range(kstart,min([kend+1,len(times)])):
			ti = times[i];	
			T.append(ti,points[ti]);
		R.append(T);	
	# note: the last point of the last track is not inserted.
	# TODO: fix that.
	return R;			

def main(filename):
	tracks(filename);
	
if __name__ == '__main__':
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

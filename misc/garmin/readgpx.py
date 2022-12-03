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
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	return gpx;
		

def to_track(segment,filename):
	ret=track.Track(filename);
	for point in segment.points:
		latitude=point.latitude;
		longitude=point.longitude;
		elevation=point.elevation;
		time = point.time;
		(x,y)=projection.convert(latitude,longitude);
		ret.append(time,geometry.Point(x,y,latitude,longitude,elevation));
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
			if file.endswith(".gpx") and ("Track_" in file or "Current.gpx" in file):
				filename=os.path.join(root, file);
				try:	
					ret.extend(tracks(filename));
				except Exception as e:
					#print(filename,e)
					pass;	
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
		# split if two points are 1800 secs apart
		if d > 1800:
			ends.append(k);
	kstart=0;
	kend=-1;
	# UTM32 coordinates in front of your door.
	home=geometry.Point(555235,5317262);
	while True:
		kstart=kend+1;
		if kstart >= len(times):
			break;	
		if ends: # not empty
			kend=ends.pop(0);
		else:
			kend=len(times);
		name0=times[kstart].strftime("%Y.%m.%d")
		name=f"{name0:s}-{len(R):03d}";
		T0=track.Track(name);
		for i in range(kstart,min([kend+1,len(times)])):
			ti = times[i];
			T0.append(ti,points[ti]);
		shrinktimes=sorted(T0.points().keys());
		G=T0.points();
		while(shrinktimes and home.distance(G[shrinktimes[0]])<200):
			shrinktimes.pop(0);
		while(shrinktimes and home.distance(G[shrinktimes[-1]])<200):
			shrinktimes.pop(-1);
		T=track.Track(name);
		for t in shrinktimes:
			T.append(t,G[t]);
		if T.points():	
			R.append(T);	
	# note: the last point of the last track is not inserted.
	# TODO: fix that.
	return R;

def write(track,filename):
	gpx = gpxpy.gpx.GPX();
	gpx_track = gpxpy.gpx.GPXTrack()
	gpx.tracks.append(gpx_track)
	gpx_segment = gpxpy.gpx.GPXTrackSegment()
	gpx_track.segments.append(gpx_segment)
	# Create points:
	P=track.points();
	for t in P:
		lat=P[t].latitude();	
		lon=P[t].longitude();
		elev=P[t].elevation();
		time=t;
		gpx_segment.points.append(gpxpy.gpx.GPXTrackPoint(lat,lon,elev,time));
	f=open(filename,'w');	
	f.write(gpx.to_xml());
	f.close();

def main(filename):
	tracks(filename);
	
if __name__ == '__main__':
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

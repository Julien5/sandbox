#!/usr/bin/env python3

import track;
import geometry;

import gpxpy
import gpxpy.gpx
import sys;
import os;
import datetime;
import xml.etree.cElementTree as mod_etree

import projection;
import statistics;


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
		assert(time);
		(x,y)=projection.convert(latitude,longitude);
		ret.append(geometry.Point(x,y,latitude,longitude,elevation,time));
	return ret;

def tracks(filename):
	gpx = readgpx(filename);
	ret = list();
	for T in gpx.tracks:
		for S in T.segments:
			cleansegment(S);
			ret.append(to_track(S,filename));
	return ret;

def old(filename):
	return False;
	now=datetime.datetime.now();
	filetime=datetime.datetime.fromtimestamp(os.path.getmtime(filename));
	d=now-filetime;
	return d.days > 30;

def tracksfromdir(dirname):
	ret=list();
	for root, dirs, files in os.walk(dirname):
		for file in files:
			if file.endswith(".gpx") and ("Track_" in file or "Current.gpx" in file):
				filename=os.path.join(root, file);
				if old(filename):
					continue;
				try:	
					ret.extend(tracks(filename));
				except Exception as e:
					print("ERROR",filename,"->",e)
					#assert(0);
					pass;	
	return ret;

def home(T):
	points=list();	
	for t in T:
		points.extend(t.geometry()[0:5]);
	x=statistics.median([p.x() for p in points]);
	y=statistics.median([p.y() for p in points]);
	return geometry.Point(x,y);	

def clean(tracks):
	assert(tracks);
	# first gather all distinct points
	points=dict();
	for T in tracks:
		P=T.points();
		for p in P:
			assert(p);
			points[p.time()]=p;
	assert(points);		
	# as list
	times=sorted(list(points));
	R=list();
	T=None;
	N=len(times);
	ends=list();
	homepoint=home(tracks);
	for k in range(N-1):
		t0=times[k];
		t1=times[k+1];
		d=(t1-t0).total_seconds();
		# there is normally several points per minutes
		# (2 points/minutes is the most common).
		# sometimes, there is no point for 1 minute.
		# no points for 10 minutes -> cut.
		if d > 600:
			p0=points[t0];
			p1=points[t1];
			distance=p0.distance(p1);
			if d < 3600 and distance<100 and points[t0].distance(homepoint)>1000:
				# if the gap is less than one hour and there was no movement
				# and i am far from home	
				# => this is a break and i turned off the GPS.
				print("[coffee] time gap:",t0,t1,d,"distance:",distance);
			else:
				#print("[cut] time gap:",t0,t1,d);
				ends.append(k);
		elif d > 200:
			print("[warning] large time gap:",t0,t1,d);
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
		name0=times[kstart].strftime("%Y.%m.%d")
		name=f"{name0:s}-{len(R):03d}";
		T0=track.Track(name);
		for k in range(kstart,min([kend+1,len(times)])):
			T0.append(points[times[k]]);
		G=T0.points();
		threshold=50
		# remove points near the house, considering they dont *really* belong
		# to get the most accurage (and comparable) average speed data.
		startpoint = G[0];
		stoppoint = G[-1];
		while(G and startpoint.distance(G[0])<threshold):
			G.pop(0);
		while(G and stoppoint.distance(G[-1])<threshold):
			G.pop(-1)
		T=track.Track(name);
		for g in G:
			T.append(g);
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
	for p in P:
		lat=p.latitude();	
		lon=p.longitude();
		elev=p.elevation();
		time=p.time();
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

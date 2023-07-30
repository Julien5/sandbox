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

def isOAWaypoint(p):
	if not p.name:
		return False;
	if p.description:
		return True;
	C=p.name.split(",");
	if len(C) != 2:
		return True;
	try:
		float(C[0]);
		float(C[1]);
	except ValueError:
		# not a float
		return True;
	return False;

def toWaypoint(p):
	w = gpxpy.gpx.GPXWaypoint()
	w.latitude=p.latitude;
	w.longitude=p.longitude;
	w.elevation=p.elevation;
	w.time=p.time;
	w.name=p.name;
	w.description=p.description;
	w.symbol = "Flag, Blue";
	return w;
	
def getOAWaypoints(segment):
	N=len(segment.points);
	ret=list();
	for i in range(N):
		p=segment.points[i];
		if isOAWaypoint(p):
			ret.append(toWaypoint(p));
	return ret;

def remove_time_and_elevation(segment):
	N=len(segment.points);
	ret=list();
	out=gpxpy.gpx.GPXTrackSegment();
	for i in range(N):
		p=segment.points[i];
		p2=gpxpy.gpx.GPXTrackPoint()
		p2.latitude=p.latitude;
		p2.longitude=p.longitude;
		out.points.append(p2);
	return out;	

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
		#assert(time);
		(x,y)=projection.convert(latitude,longitude);
		ret.append(geometry.Point(x,y,latitude,longitude,elevation,time));
	return ret;

import pickle;
import hashfile;
def tracks(filename):
	Hash=hashfile.get(filename);
	cache_file="cache/"+Hash;
	if os.path.exists(cache_file):
		with open(cache_file, 'rb') as f:
			return pickle.load(f);
	gpx = readgpx(filename);
	ret = list();
	for T in gpx.tracks:
		print("reading",os.path.basename(filename));
		for S in T.segments:
			cleansegment(S);
			ret.append(to_track(S,filename));
	with open(cache_file, 'wb') as f:
		pickle.dump(ret, f);
	assert(os.path.exists(cache_file));	
	return ret;

import hashfile;
def tracksfromdir(dirname):
	D=dict();
	for root, dirs, files in os.walk(dirname):
		for file in files:
			if "!" in file:
				continue;
			if file == "Current.gpx":
				continue;
			if file.endswith(".gpx") and ("Track_" in file or "Current.gpx" in file):
				filename=os.path.join(root, file);
				H=hashfile.get(filename);
				if H in D:
					f1=D[H]
					f2=filename;
					ok=os.path.basename(f1)==os.path.basename(f2);
					if not ok:
						print(f1);
						print(f2);
						assert(False);
				D[H]=filename;

	ret=list();
	for filename in D.values():		
		try:
			#print("read",filename);
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

def clean_singletrack(track):
	track.strip();
	return track;

def clean(tracks):
	assert(tracks);
	R=list();
	for t in tracks:
		c=clean_singletrack(t)
		if not c.empty():
			R.append(c);
	R=sorted(R, key=lambda track: track.endtime())
	return R;	
	
def writegpx(filename,gpx):
	f=open(filename,'w');	
	f.write(gpx.to_xml());
	f.close();

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
	writegpx(filename,gpx);	

def main(filename):
	tracks(filename);
	
if __name__ == '__main__':
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

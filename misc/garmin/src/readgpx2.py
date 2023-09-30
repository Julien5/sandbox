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
			#if file == "Current.gpx":
			#	continue;
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

def install(T):
	return;

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

import writegpx;

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
			print("hit cache",filename);
			return pickle.load(f);
	gpx = readgpx(filename);
	ret = list();
	for T in gpx.tracks:
		print("reading",os.path.basename(filename));
		for S in T.segments:
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

def autoclean(t):
	t2=track.Track(t.name(),t.points());
	t2.strip();
	return t2;

class EmptySegment(Exception):
    pass

def starttime(track):
	striped=autoclean(track);
	P=striped.points();
	if not P:
		raise EmptySegment();
	starttime=P[0].time()+datetime.timedelta(hours=2);
	return starttime;

def dirname(track):
	time=fixUTC(starttime(track));
	D1=time.strftime("%Y.%m.%d");
	D2=time.strftime("%H.%M.%S");
	return os.path.join("readgpx2",D1,D2);

#def mkdir(track):
#	D=dirname(track);
#	for d in ["raw","auto","clean","data"]:
#		os.makedirs(D+"/"+d);
#	return D;

def write(t,path):
	os.makedirs(os.path.dirname(path),exist_ok=True);
	writegpx.write(t,path);

def	fixUTC(time):
	return time+datetime.timedelta(hours=2);

def meta(t):
	raw={};
	raw["name"]=t.name();
	raw["points"]=str(len(t.points()));
	raw["distance"]=str(t.distance());
	P=t.points();
	if P:
		raw["start"]=fixUTC(P[0].time()).strftime("%Y.%m.%d %H:%M:%S");
		raw["end"]=fixUTC(P[-1].time()).strftime("%Y.%m.%d %H:%M:%S");
	return raw;

def writemeta(t,clean,path):
	os.makedirs(os.path.dirname(path),exist_ok=True);
	f=open(path,'w');
	for pair in [("raw",meta(t)),("clean",meta(clean))]:
		(name,content)=pair;
		f.write(f"** {name:10s}\n");
		f.write("\n".join(f"{key:20s} {content[key]:s}" for key in content));
		f.write("\n\n");
	f.close();
	

def install(T):
	dir="readgpx2";
	for t in T:
		try:
			write(t,dirname(t)+"/raw/track.gpx");
			clean=autoclean(t);
			write(clean,dirname(t)+"/auto/track.gpx");
			writemeta(t,clean,dirname(t)+"/meta/info.txt");
		except EmptySegment:
			print("empty segment in",t.name(),"=> skip it");
		

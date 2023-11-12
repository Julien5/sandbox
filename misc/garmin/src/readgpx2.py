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
	cache_file=cache_file_hash(Hash);
	if os.path.exists(cache_file):
		with open(cache_file, 'rb') as f:
			# print("hit cache",filename);
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

def autoclean(t):
	t2=track.Track(t.name(),t.points());
	t2.strip();
	return t2;

class EmptySegment(Exception):
    pass

class NotRecordedTrack(Exception):
    pass

def throw_if_bad(clean):
	P=clean.points();
	if not P:
		raise EmptySegment();
	if P[0].time() is None:
		raise NotRecordedTrack();
	if not clean.duration().total_seconds():
		raise EmptySegment();
	

def starttime(track):
	striped=autoclean(track);
	P=striped.points();
	if not P:
		raise EmptySegment();
	if P[0].time() is None:
		raise NotRecordedTrack();
	starttime=P[0].time();
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

# name = auto or manual
def read_installed_path(path,name):
	cache_file=cache_file_gpx(path);
	if os.path.exists(cache_file):
		with open(cache_file, 'rb') as f:
			return pickle.load(f);
	gpx = readgpx(path);
	assert(len(gpx.tracks)==1);
	assert(len(gpx.tracks[0].segments)==1);
	ret=to_track(gpx.tracks[0].segments[0],path);
	D=os.path.dirname(path);
	writemeta(ret,D+f"/../meta/{name:s}.txt");
	with open(cache_file, 'wb') as f:
		pickle.dump(ret, f);
	return ret;	

def	fixUTC(time):
	return time+datetime.timedelta(hours=2);

def read_meta_fromfile(t):
	path=t.name();
	D=os.path.dirname(path);
	name=os.path.basename(D);
	filename=os.path.join(D,"..","meta",name+".txt");
	L=open(filename,"r").readlines();
	ret={};
	for line in L:
		key=line[0:20].strip();
		value=line[21:-1].strip();
		if not key:
			continue;
		ret[key]=value;
		if key in {"start","end"}:
			ret[key]=datetime.datetime.strptime(value,"%Y.%m.%d %H:%M:%S");
		elif key in {"distance"}:
			ret[key]=int(value);
		elif key in {"points"}:
			ret[key]=int(value);
	return ret;	

def meta_computed(t):
	raw={};
	raw["name"]=t.name();
	raw["points"]=len(t.points());
	raw["distance"]=int(t.distance());
	raw["category"]=t.category();
	P=t.points();
	if P:
		raw["start"]=fixUTC(P[0].time());
		raw["end"]=fixUTC(P[-1].time());
	return raw;

def meta(t):
	try:
		return read_meta_fromfile(t);
	except FileNotFoundError as e:
		print(e);
		pass;
	return meta_computed(t);

def writemeta(t,path):
	os.makedirs(os.path.dirname(path),exist_ok=True);
	f=open(path,'w');
	M=meta_computed(t);
	L=list();
	ret={};
	for k in M:
		key=k;
		value=M[k];
		ret[key]=str(value);
		if key in {"start","end"}:
			ret[key]=value.strftime("%Y.%m.%d %H:%M:%S");
	f.write("\n".join(f"{key:20s} {ret[key]:s}" for key in ret));
	f.write("\n\n");
	f.close();

def readmeta(t,qualifier="auto"):
	D=dirname(t);
	filename=D+f"/meta/{qualifier:s}.txt"
	

def installed(t):
	return os.path.exists(dirname(t)+"/meta/info.txt");
	
def install(T):
	create_cache_directories();
	dir="readgpx2";
	ret=list();
	for t in T:
		try:
			if installed(t):
				continue;
			D=dirname(t);
			clean=autoclean(t);
			throw_if_bad(clean);
			write(t,D+"/raw/track.gpx");
			write(clean,D+"/auto/track.gpx");
			writemeta(t,D+"/meta/raw.txt");
			writemeta(clean,D+"/meta/auto.txt");
			ret.append(D);
		except EmptySegment:
			pass;
			print("empty segment in",t.name(),"=> skip it");
		except NotRecordedTrack:
			pass;
			print("not recorded track in",t.name(),"=> skip it");
	return ret;	

def read_installed_tour(dir):
	for name in ["manual","auto"]:
		d=os.path.join(dir,name);
		gpx=os.path.join(d,"track.gpx");
		if os.path.exists(gpx):
			# print("read",gpx);
			return read_installed_path(gpx,name);
	print("failed",dir);
	assert(0);

def read_all_installed():
	dir="readgpx2";
	ret=list();
	for d1 in sorted(os.listdir(dir)):
		for d2 in sorted(os.listdir(os.path.join(dir,d1))):
			root=os.path.join(dir,d1,d2);
			ret.append(read_installed_tour(root));
	return ret;

def date(filename):
	if filename:
		return os.path.getctime(filename);
	return None;

def latest_file(dir):
	L=list();
	for root, dirs, files in os.walk(dir):
		for f in files:
			filename=os.path.join(root,f);
			L.append(filename);
	if L:
		return max(L, key=os.path.getctime);
	return None;

def recent_files(dir,date):
	L=list();
	for root, dirs, files in os.walk(dir):
		for f in files:
			filename=os.path.join(root,f);
			if not is_relevant_track(filename):
				continue;
			d=os.path.getctime(filename);
			if date is None:
				L.append(filename);
			elif d > date:
				L.append(filename);
	return L;

def newfilesfromdir(rawdir):
	create_cache_directories();
	latest_dst=latest_file("readgpx2");
	d=date(latest_dst);
	return recent_files(rawdir,d);
	

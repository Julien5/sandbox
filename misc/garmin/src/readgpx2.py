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

def cache_root():
	HOME=os.path.expanduser('~');
	return os.path.join(HOME,".cache","readgpx2");

def cache_file_hash(Hash):
	return os.path.join(cache_root(),"hash",Hash);

def cache_root_gpx():
	return os.path.join(cache_root(),"gpx");

def cache_file_gpx(gpx):
	assert(not os.path.isabs(gpx));
	D=gpx.split("/");
	# readgpx2/2022.07.10/12.20.57/auto/track.gpx
	# => 2022.07.10/12.20.57/auto
	assert(len(D)==5);
	dir=os.path.join(D[1],D[2],D[3]);
	cache_dir=os.path.join(cache_root_gpx(),dir);
	filename=os.path.basename(gpx);
	cache_file=filename.replace(".gpx",".cache");
	os.makedirs(cache_dir,exist_ok=True);
	return os.path.join(cache_dir,cache_file);

def create_cache_directories():
	for a in ["gpx","hash"]:
		os.makedirs(os.path.join(cache_root(),a),exist_ok=True);
	
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

def is_relevant_track(f):
	b=os.path.basename(f);
	return b.endswith(".gpx") and ("Track_" in b or "Current.gpx" in b);

def unique_new_files(dirname):
	# ignore old files 
	# remove duplicate files (content wise)
	Recent=newfilesfromdir(dirname);
	print(f"there are {len(Recent):d} new files");
	D=dict();
	for root, dirs, files in os.walk(dirname):
		for file in files:
			filename=os.path.join(root, file);
			if not filename in Recent:
				continue;
			if "!" in file:
				continue;
			#if file == "Current.gpx":
			#	continue;
			if is_relevant_track(file):
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
	return D.values();		

import hashfile;
def newtracksfromdir(dirname):
	D=dict();
	ret=list();
	for filename in unique_new_files(dirname):		
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
	cache_file=cache_file_gpx(path);
	with open(cache_file, 'wb') as f:
		pickle.dump(t, f);

def read_installed_path(path):
	cache_file=cache_file_gpx(path);
	if os.path.exists(cache_file):
		with open(cache_file, 'rb') as f:
			print(" -> hit",cache_file);
			return pickle.load(f);
	gpx = readgpx(path);
	ret = list();
	assert(len(gpx.tracks)==1);
	assert(len(gpx.tracks[0].segments)==1);
	return to_track(gpx.tracks[0].segments[0],path);

def	fixUTC(time):
	return time+datetime.timedelta(hours=2);

def meta(t):
	raw={};
	raw["name"]=t.name();
	raw["points"]=str(len(t.points()));
	raw["distance"]=str(t.distance());
	raw["category"]=t.category();
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
			writemeta(t,clean,D+"/meta/info.txt");
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
			print("read",gpx);
			return read_installed_path(gpx);
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
	

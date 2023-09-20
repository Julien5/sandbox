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

def home(T):
	points=list();	
	for t in T:
		points.extend(t.geometry()[0:5]);
	x=statistics.median([p.x() for p in points]);
	y=statistics.median([p.y() for p in points]);
	return geometry.Point(x,y);

def long_enough(track):
	if track.empty():
		return False;
	G=track.points();
	(xmin,xmax,ymin,ymax)=track.bbox();
	dx=xmax-xmin;
	dy=ymax-ymin;
	if max(dx,dy)<1000:
		return False;
	duration=track.duration();
	if duration.total_seconds()<600:
		return False;
	return True;

def subsegments(t,maxdelta):
	G=t.points();
	L=len(G);
	assert(L);
	K=list();
	for k in range(L-1):
		p0=G[k];
		p1=G[k+1];
		delta=p1.time()-p0.time();
		if delta.total_seconds()>maxdelta:
			K.append(k);
	K.append(L);
	ret=list();
	begin=0;
	while K:
		end=K.pop(0);
		Gloc=G[begin:end];
		Tloc=track.Track(t.name(),Gloc);
		Tloc.strip();
		if long_enough(Tloc):
			ret.append(Tloc);
		else:
			length=len(Tloc.points());
			if length>1:
				print("remove short segment in",t.name(),"length:",length,Tloc.begintime());
		begin=end;
	return ret;
	
def clean_singletrack(track):
	track.strip();
	return track;

def savetmp(t,n):
	dir="/tmp/"+t.begintime().strftime("%Y.%m.%d");
	if not os.path.exists(dir):
		os.mkdir(dir);
	gpx=dir+"/"+t.category()+"-"+str(n)+".gpx";
	txt=dir+"/"+t.category()+"-"+str(n)+".txt";
	write(t,gpx);
	open(txt,'w').write(t.name()+"\n")

def remove_duplicates_(R):
	D=dict();
	for t in R:
		beg=t.begintime();
		if beg in D:
			t1=D[beg];
			t2=t;
			assert(t1.begintime()==t2.begintime());
			if t2.duration()>t1.duration():
				D[beg]=t2;
		else:
			D[beg]=t;
	return sorted(D.values(), key=lambda track: track.begintime());

def remove_duplicates(R):
	D=dict();
	for t in R:
		G=t.points();
		for p in G:
			if not p.time() in D:
				D[p.time()]=list();
			D[p.time()].append(t.name());
	for t in D:
		if len(D[t])>1:
			print(D[t]);

	D=dict();		
	for t in R:
		G=t.points();
		for p in G:
			D[p.time()]=p;
			
	P=list();
	for time in sorted(D):
		P.append(D[time]);
	t=track.Track("all",P);
	return [t];

def clean(_tracks):
	assert(_tracks);
	R=list();
	T=sorted(_tracks, key=lambda track: track.begintime());
	T=remove_duplicates(T);
	for k in range(len(T)):
		t=T[k];
		c=clean_singletrack(t);
		if not c.empty():
			S=subsegments(c,600);
			if len(S)>1:
				print("file",c.name(),"#segments:",len(S));
			for s in S:
				if not s.sanity_check():
					print("unsane",t.name());
				else:
					savetmp(s,len(R));
					R.append(s);
		else:
			print("empty",t.name());
	R=sorted(R, key=lambda track: track.begintime());
	k=0;
	while k+1<len(R):
		d=R[k+1].begintime()-R[k].endtime();
		print(R[k].begintime(),R[k].endtime());
		print(R[k+1].begintime(),R[k+1].endtime());
		print(k,k+1,d);
		assert(d.total_seconds()>=0);
		if d.total_seconds()<600:
			print("merge:");
			print(R[k].name());
			print(R[k+1].name());
			R[k].append_subtrack(R[k+1]);
			R.pop(k+1);
		else:
			k=k+1;
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

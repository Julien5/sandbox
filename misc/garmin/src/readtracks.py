#!/usr/bin/env python3

import track;
import geometry;

import gpxpy
import gpxpy.gpx

import os;
import sys;
import datetime;
import copy;
import math;
import builtins;

import output;
import pickle;


def readgpx(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	return gpx;

def writegpx(filename,gpx):
	f=open(filename,'w');	
	f.write(gpx.to_xml());
	f.close();

class Point:
	def __init__(self,la,lo,ele,time):
		self.latitude=la;
		self.longitude=lo;
		self.elevation=ele;
		self.time=time;

def PointFromGPXPY(p):
	return Point(p.latitude,p.longitude,p.elevation,p.time);
	
def writepoints(points,filename):
	gpx = gpxpy.gpx.GPX();
	gpx_track = gpxpy.gpx.GPXTrack()
	gpx.tracks.append(gpx_track)
	gpx_segment = gpxpy.gpx.GPXTrackSegment()
	gpx_track.segments.append(gpx_segment)
	# Create points:
	for p in points:
		lat=p.latitude;	
		lon=p.longitude;
		elev=p.elevation;
		time=p.time;
		gpx_segment.points.append(gpxpy.gpx.GPXTrackPoint(lat,lon,elev,time));
	writegpx(filename,gpx);	
			
def segpoints(segment):
	ret=list();
	for point in segment.points:
		assert(point.time);
		p=PointFromGPXPY(point);
		ret.append(p);
	return ret;

def readpoints(path):
	gpx = readgpx(path);
	ret=list();
	for track in gpx.tracks:
		for seg in track.segments:
			ret.extend(segpoints(seg));
	return ret;		

def distance(p1,p2):
	import geopy.distance
	pp1=(p1.latitude,p1.longitude);
	pp2=(p2.latitude,p2.longitude);
	return 1000*geopy.distance.geodesic(pp1,pp2).km;

def time_seconds(p1,p2):
	return (p2.time-p1.time).total_seconds();

def movement(points,n):
	d=0;
	t=0;
	speed=0;
	if n >= len(points)-1:
		return (d,t,speed);
	p1=points[n];
	p2=points[n+1];
	d=distance(p1,p2);
	t=time_seconds(p1,p2);
	if t != 0:
		speed=d/t;
	return (d,t,speed);

class Interval:
	def __init__(self,typename,begin,end):
		self.begin=begin;
		self.end=end;
		self.typename=typename;
		assert(not self.begin is None);
		assert(not self.end is None);

	def contains(self,n):
		return self.begin<=n and n<self.end;

	def duration(self,points):
		return points[self.end].time - points[self.begin].time;

	def __str__(self):
		return f"{self.begin:d}-{self.end:d}";

class StartTimeComparator:
	def __init__(self,points):
		self.points=points;

	def key(self,interval):
		return self.points[interval.begin].time;

class NoInterval:
	def __init__(self,I):
		self.intervals=copy.deepcopy(I);

	def get(self,points,n):
		for interval in self.intervals:
			if interval.contains(n):
				return False;
		return True;

class LongPauses:
	def __init__(self,points,IP):
		self.minpause=datetime.timedelta(minutes=120);
		self.intervals=copy.deepcopy(IP);

	def get(self,points,n):
		for interval in self.intervals:
			if interval.contains(n) and interval.duration(points)>self.minpause:
				return True;
		return False;	


def pause_condition(points,n):
	(d,dt,speed)=movement(points,n);
	kmh=3600*speed/1000;
	return kmh<3;

def gap_condition(points,n):
	N=len(points);
	if n>=(N-1):
		return False;
	p1=points[n];
	p2=points[n+1];
	dt=p2.time - p1.time;
	maxdt=datetime.timedelta(hours=3);
	return dt>maxdt;

#def moving_condition(points,n):
#	return not pause_condition(points,n) and not gap_condition(points,n);

def apply_intervals(points,intervals):
	ret=list();
	for I in intervals:
		begin=I.begin;
		end=I.end;
		#if I.typename=="gap":
		#	end=I.end+1;
		p2=points[begin:end]
		ret.append(p2);
	return ret;

def findIntervals(typename,points,condition):
	assert(points);
	begin=None;
	end=None;
	intervals=list();
	N=len(points);
	for n in range(N):
		inside=condition(points,n);
		if begin is None and inside: # get in
			begin=n;
		if (not inside and not begin is None) or n==(N-1): # get out
			if n==(N-1) and begin is None:
				# did not find any interval
				continue;
			end=n;
			assert(not begin is None);
			assert(end);
			intervals.append(Interval(typename,begin,end));
			begin=None;
			end=None;
	return intervals;

def serialize_stats(D):
	return pickle.dumps(D);

def deserialize_stats(data):
	return pickle.loads(data);

def writestats(D,filename):
	f=open(filename,'wb');
	f.write(serialize_stats(D));
	
def readstats(filename):
	f=open(filename,'rb');
	deserialize_stats(f.read());

def statistics(points,start=None,end=None):
	ret={};
	N=len(points);
	distance=0;
	moving_seconds=0;
	minspeed=4*1000.0/3600; # m/s
	if start is None:
		start=0;
	if end is None:
		end=N;
	for k in range(start,end):
		(d,t,speed)=movement(points,k);
		distance += d;
		if speed > minspeed:
			moving_seconds += t;
	seconds=time_seconds(points[start], points[end-1]);
	ret["distance"]	= distance;
	ret["duration"] = points[end-1].time - points[start].time;
	ret["seconds"] = seconds;
	ret["meanspeed"] = 0;
	ret["movingspeed"] = 0;
	ret["start"]=start;
	ret["end"]=end;
	ret["startpoint"]=points[start];
	ret["endpoint"]=points[end-1];
	ret["N"]=end-start;
	if seconds>0:
		ret["meanspeed"] = distance/seconds;
	if moving_seconds>0:
		ret["movingspeed"] = distance/moving_seconds;
	return ret;

def print_statistics(S,name):
	startdate=S["startpoint"].time.strftime("%d.%m.%Y (%a)");
	starttime=(S["startpoint"].time+datetime.timedelta(hours=2)).strftime("%H:%M");
	enddate=S["endpoint"].time.strftime("%d (%a)");
	endtime=(S["endpoint"].time+datetime.timedelta(hours=2)).strftime("%H:%M");

	print(f"{name:10s}",end="| ");
	N=S["N"];
	print(f"{N:4d}",end=" |");
	start=S["start"];
	end=S["end"];
	print(f"{start:4d}",end="-");
	print(f"{end:4d}",end=" |");
	print(f"{startdate:8s}",end=" ");
	print(f"{starttime:5s}",end=" - ");
	print(f"{enddate:5s}",end=" - ");
	print(f"{endtime:5s}",end=" |");
	distance=S["distance"];
	print(f"{distance/1000:5.1f} km",end=" | ");
	ds=S["seconds"];
	hours=math.floor(ds/3600);
	seconds=ds-3600*hours;
	minutes=math.floor(seconds/60);
	seconds=ds-3600*hours-60*minutes;
	print(f"{hours:02d}:{minutes:02d}:{int(seconds):02d}",end=" | ");
	speed=3600*S["meanspeed"]/1000;
	mspeed=3600*S["movingspeed"]/1000;
	print(f"{speed:4.1f} kmh",end=" |");
	print(f"{mspeed:4.1f} kmh",end=" |");	
	#print(f"{track.name():s}",end=" |");
	print("");

def process(directory):
	origin=os.path.join(directory,"gpx","origin.gpx");
	print(origin)
	points=readpoints(origin);
	assert(points);
	print("N=",len(points));

	stats=statistics(points);
	print_statistics(stats,"all");
	
	I=[];
	Ipauses=findIntervals("tmp",points,pause_condition);
	longPausesFinder=LongPauses(points,Ipauses);
	
	I.extend(findIntervals("pauses",points,longPausesFinder.get));
	I.extend(findIntervals("gap",points,gap_condition));
	
	noIntervalFinder=NoInterval(I);
	I.extend(findIntervals("moving",points,noIntervalFinder.get));
	
	startTimeCompare=StartTimeComparator(points);
	I_sorted=sorted(I,key=startTimeCompare.key);

	MovingIntervals=list();
	subtracks=apply_intervals(points,I_sorted);
	assert(len(subtracks)==len(I_sorted));
	N=len(subtracks);
	for n in range(N):
		interval=I_sorted[n];
		typename=interval.typename;
		if typename != "moving":
			# skip
			continue;
		begin=interval.begin;
		end=interval.end;
		track=subtracks[n];
		gpxfilename=f"{n:02d}-{interval.typename:s}.gpx";
		writepoints(track,os.path.join(directory,"gpx",gpxfilename));
		statsfilename=f"{n:02d}-{interval.typename:s}.txt";
		stats=statistics(points,begin,end);
		if stats["distance"]<1000:
			# skip
			continue;
		s=serialize_stats(stats);
		s2=deserialize_stats(s);
		writestats(stats,os.path.join(directory,"gpx",statsfilename));
		print_statistics(stats,typename);
		
def processtrack(directory):
	if not os.path.exists(os.path.join(directory,"subtracks")):
		name=os.path.basename(directory)[:6];
		(points,S)=makesubtracks(directory);
		print_statistics(statistics(points),name);
		for k in range(len(S)):
			points=S[k];
			name_k = f"{name:s}-{k:d}";
			print_statistics(statistics(points),name_k);

def main():
	if len(sys.argv)>1:
		dirname=sys.argv[1];
	else:
		#dirname="/home/julien/projects/tracks/a86483799d2afca78bde6e1662ad8281";
		#dirname="/home/julien/projects/tracks/ba7d58fad784a0270d47cda199971a00"
		#dirname="/home/julien/projects/tracks/bb2cc714674ef5fdb0c0423a593e0344"
		#dirname="/home/julien/projects/tracks/83fe50d8f8407a830507cd5ecfd0ce25"
		#dirname="/home/julien//projects/tracks/0378f791b6ff5cbfdd575600aca03ae5"
		dirname="/home/julien//projects/tracks/0829577e9ff09026f7ae0d9e7eb30add"
	process(dirname);

if __name__ == "__main__":
	main();	

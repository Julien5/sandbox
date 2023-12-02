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
	return deserialize_stats(f.read());

class Statistics:
	def __init__(self,directory,points,interval=None):
		N=len(points);
		distance=0;
		moving_seconds=0;
		minspeed=4*1000.0/3600; # m/s
		if interval is None:
			interval=Interval("all",0,N);
		for k in range(interval.begin,interval.end):
			(d,t,speed)=movement(points,k);
			distance += d;
			if speed > minspeed:
				moving_seconds += t;
		seconds=time_seconds(points[interval.begin], points[interval.end-1]);
		self.directory = directory;
		self.typename	= interval.typename;
		self.distance	= distance;
		self.duration= points[interval.end-1].time - points[interval.begin].time;
		self.seconds= seconds;
		self.meanspeed= 0;
		self.movingspeed= 0;
		self.start = interval.begin;
		self.end = interval.end;
		self.startpoint=points[interval.begin];
		self.endpoint=points[interval.end-1];
		self.N =interval.end-interval.begin;
		if seconds>0:
			self.meanspeed= distance/seconds;
		if moving_seconds>0:
			self.movingspeed= distance/moving_seconds;
		

def category_speed(kmh,threshold):
	if kmh>threshold:
		return "cycling";
	return "running";

def category(S):
	if S.typename != "moving":
		return "none";
	distance=S.distance;
	km=distance/1000;
	kmh=3600*S.movingspeed/1000;
	if km>40:
		return category_speed(kmh,10);
	if km>15:
		return category_speed(kmh,15);
	if km>10:
		return category_speed(kmh,18);
	return category_speed(kmh,20);
	
def print_statistics(S):
	startdate=S.startpoint.time.strftime("%d.%m.%Y (%a)");
	starttime=(S.startpoint.time+datetime.timedelta(hours=2)).strftime("%H:%M");
	enddate=S.endpoint.time.strftime("%d (%a)");
	endtime=(S.endpoint.time+datetime.timedelta(hours=2)).strftime("%H:%M");

	name=S.typename;
	cat=category(S);
	print(f"{cat:8s}",end="| ");
	#N=S.N;
	#print(f"{N:4d}",end=" |");
	#start=S.start;
	#end=S.end;
	#print(f"{start:4d}",end="-");
	#print(f"{end:4d}",end=" |");
	print(f"{startdate:8s}",end=" ");
	print(f"{starttime:5s}",end=" - ");
	print(f"{endtime:5s}",end=" |");
	distance=S.distance;
	print(f"{distance/1000:5.1f} km",end=" | ");
	ds=S.seconds;
	hours=math.floor(ds/3600);
	seconds=ds-3600*hours;
	minutes=math.floor(seconds/60);
	seconds=ds-3600*hours-60*minutes;
	print(f"{hours:02d}:{minutes:02d}:{int(seconds):02d}",end=" | ");
	speed=3600*S.meanspeed/1000;
	mspeed=3600*S.movingspeed/1000;
	print(f"{speed:4.1f} kmh",end=" |");
	print(f"{mspeed:4.1f} kmh",end=" |");	
	#print(f"{track.name():s}",end=" |");
	print("");

def process(directory):
	origin=os.path.join(directory,"gpx","origin.gpx");
	points=readpoints(origin);
	assert(points);

	stats=Statistics(directory,points);
	writestats(stats,os.path.join(directory,"gpx","all.txt"));
	
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
		stats=Statistics(directory,points,interval);
		if stats.distance<1000:
			# skip
			continue;
		s=serialize_stats(stats);
		s2=deserialize_stats(s);
		writestats(stats,os.path.join(directory,"gpx",statsfilename));

from glob import glob
def movingstatsfiles(dirname):
	return glob(dirname+"/**/*-moving.txt", recursive=True);

def readallstats():
	D={};
	for dirname in glob("/home/julien/projects/tracks/a*/", recursive=False):
		alltxt=os.path.join(dirname,"gpx","all.txt")
		if not os.path.exists(alltxt):
			process(dirname);

		stats=readstats(alltxt);
		D[stats.startpoint.time]=readstats(alltxt);
		for filename in movingstatsfiles(dirname):
			stats=readstats(filename);
			D[stats.startpoint.time]=stats;
			
	for time in sorted(D.keys()):
		s=D[time];
		print_statistics(s);
		
def main():
	readallstats();

if __name__ == "__main__":
	main();	

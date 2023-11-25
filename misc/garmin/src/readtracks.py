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

import output;

def readgpx(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	return gpx;

class Point:
	def __init__(self,latitude,longitude=None,elevation=None,time=None):
		if time:
			self.latitude=latitude;
			self.longitude=longitude;
			self.elevation=elevation;
			self.time=time;
		else:
			p=latitude;
			self.latitude=p.latitude;
			self.longitude=p.longitude;
			self.elevation=p.elevation;
			self.time=p.time;
		
def segpoints(segment):
	ret=list();
	for point in segment.points:
		assert(point.time);
		ret.append(Point(point));
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

def state(points,n):
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
	def __init__(self,begin,end):
		self.begin=begin;
		self.end=end;

	def __str__(self):
		return f"{self.begin:d}-{self.end:d}";

class Pause:
	def __init__(self,interval,points):
		self._interval=interval;
		start=points[interval.begin];
		end=points[interval.end];
		self._duration=end.time - start.time;

	def interval(self):
		return self._interval;
		
	def duration(self):
		return self._duration;

class Gap:
	def __init__(self,interval,points):
		self.interval=interval;
		self.start=points[interval.begin];
		self.end=points[interval.end];

	def duration(self):
		return self.end.time - self.start.time;

def pause_condition(points,n):
	(d,t,speed)=state(points,n);
	kmh=3600*speed/1000;
	return kmh<3; 

def find3hGaps(points):
	gaps=list();
	for n in range(len(points)-1):
		p1=points[n];
		p2=points[n+1];
		dt=p2.time - p1.time;
		maxdt=datetime.timedelta(hours=3);
		if dt>maxdt:
			gaps.append(n);
	return gaps;

def cuts(points,N):
	if not N:
		return [points];
	if N[0] != 0:
		N.insert(0,0);
	ret=list();	
	for k in range(1,len(N)):
		d=N[k]-N[k-1];
		ret.append(points[:d]);
		del points[:d];
	ret.append(points);
	return ret;

def remove_intervals(points,intervals):
	Ninit=len(points);
	ret=list();
	for I in intervals:
		N=len(points);
		delta = Ninit - N;
		begin=I.begin - delta;
		end=I.end - delta;
		section=points[:begin];
		print(f"remove {end:3d} points");	
		ret.append(points[:end]);
		del points[:end];
		if section:
			ret.append(section);
	assert(points)
	ret.append(points);	
	return ret;

def remove(points,n1,n2):
	return [points[:n1],points[n2:]];

def findIntervals(points,condition):
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
			end=n;
			intervals.append(Interval(begin,end));
			begin=None;
			end=None;
	return intervals;

def findPausingIntervals(points):
	return findIntervals(points,pause_condition);

def long_pauses(points,intervals):
	minpause=datetime.timedelta(minutes=10);
	Pauses=[Pause(i,points) for i in intervals if Pause(i,points).duration()>minpause];
	return [p.interval() for p in Pauses];

def statistics(points):
	ret={};
	N=len(points);
	distance=0;
	moving_seconds=0;
	minspeed=4*1000.0/3600; # m/s
	for k in range(N):
		(d,t,speed)=state(points,k);
		distance += d;
		if speed > minspeed:
			moving_seconds += t;
	seconds=time_seconds(points[0], points[-1]);
	ret["distance"]	= distance;
	ret["duration"] = points[-1].time - points[0].time;
	ret["seconds"] = seconds;
	ret["meanspeed"] = 0;
	ret["movingspeed"] = 0;
	if seconds>0:
		ret["meanspeed"] = distance/seconds;
	if moving_seconds>0:
		ret["movingspeed"] = distance/moving_seconds;
	return ret;

def print_statistics(points,name):
	S=statistics(points);
	startdate=points[0].time.strftime("%d.%m.%Y (%a)");
	starttime=(points[0].time+datetime.timedelta(hours=2)).strftime("%H:%M");
	enddate=points[-1].time.strftime("%d (%a)");
	endtime=(points[-1].time+datetime.timedelta(hours=2)).strftime("%H:%M");

	print(f"{name:10s}",end="| ");
	print(f"{len(points):4d}",end=" |");
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
	print(f"{hours:02d}:{minutes:02d}",end=" | ");
	speed=3600*S["meanspeed"]/1000;
	mspeed=3600*S["movingspeed"]/1000;
	print(f"{speed:4.1f} kmh",end=" |");
	print(f"{mspeed:4.1f} kmh",end=" |");	
	#print(f"{track.name():s}",end=" |");
	print("");


def strip(points):
	G=points;
	startpoint = G[0];
	stoppoint = G[-1];
	threshold=50
	while G and distance(startpoint,G[0]) < threshold:
   		G.pop(0);
	while G and distance(stoppoint,G[-1]) < threshold:
		G.pop(-1);
	return G;

def makesubtracks(directory):
	origin=os.path.join(directory,"gpx","origin.gpx");
	points=readpoints(origin);
	Nbefore=len(points);
	startbefore=points[0];
	points=strip(points);
	startafter=points[0];
	Nafter=len(points);
	dtime=startafter.time - startbefore.time;
	points0=copy.deepcopy(points);
	assert(points);
	# first the gaps.
	G=find3hGaps(points);
	P=cuts(points,G);
	assert(P);
	subtracks=list();
	npauses=0;
	for p in P:
		I=long_pauses(p,findPausingIntervals(p));
		npauses+=len(I);
		subtracks.extend(remove_intervals(p,I));
	Nsum=sum([len(s) for s in subtracks]);
	return (points0,subtracks);

		
def readtrack(directory):
	if not os.path.exists(os.path.join(directory,"subtracks")):
		name=os.path.basename(directory)[:6];
		(points,S)=makesubtracks(directory);
		states=[state(points,l) for l in range(len(points))];
		start=points[0].time;
		end=points[-1].time;
		print_statistics(points,name);
		for k in range(len(S)):
			points=S[k];
			start=points[0].time;
			end=points[-1].time;
			states=[state(S[k],l) for l in range(len(S[k]))];
			name_k = f"{name:s}-{k:d}";
			print_statistics(points,name_k);
		#print(len(S),"subtracks");

if __name__ == "__main__":
	if len(sys.argv)>1:
		dirname=sys.argv[1];
	else:
		#dirname="/home/julien/projects/tracks/a86483799d2afca78bde6e1662ad8281";
		#dirname="/home/julien/projects/tracks/ba7d58fad784a0270d47cda199971a00"
		#dirname="/home/julien/projects/tracks/bb2cc714674ef5fdb0c0423a593e0344"
		#dirname="/home/julien/projects/tracks/83fe50d8f8407a830507cd5ecfd0ce25"
		#dirname="/home/julien//projects/tracks/0378f791b6ff5cbfdd575600aca03ae5"
		dirname="/home/julien//projects/tracks/0829577e9ff09026f7ae0d9e7eb30add"
	readtrack(dirname);

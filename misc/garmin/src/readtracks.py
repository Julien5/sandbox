#!/usr/bin/env python3

import track;
import geometry;

import gpxpy
import gpxpy.gpx

import os;
import sys;
import datetime;

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

class Pause:
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

def findGaps(points):
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
		print("remove",end,"points")
		del points[:end];
		ret.append(section);
	ret.append(points);	
	return ret;

def remove(points,n1,n2):
	return [points[:n1],points[n2:]];

def findIntervals(points,condition):
	last=None;
	begin=None;
	end=None;
	intervals=list();
	#print("L:",len(points));
	for n in range(len(points)):
		inside=condition(points,n);
		# print(n,inside);
		if not begin and inside:
			#print("start (a)",n);
			begin=n;
		if inside != last and not last is None:
			if not inside:
				#print("end (b)",n);
				end=n;
				intervals.append(Interval(begin,end));
				b=points[begin].time;
				e=points[end].time;
				dmin=datetime.timedelta(minutes=10);
				duration=e-b;
				#print("begin:",begin," lasts",e-b,"#",end-begin,"points");
			else:
				#print("start (2)",n);
				begin=n;
				end=None;
		last=inside;
	if begin and not end:
		#print("end (2)",n);
		end=n;
		intervals.append(Interval(begin,end));
	return intervals;

def findPausingIntervals(points):
	return findIntervals(points,pause_condition);

def long_pauses(points,intervals):
	minpause=datetime.timedelta(hours=2);
	Pauses=[Pause(i,points) for i in intervals if Pause(i,points).duration()>minpause];
	return [p.interval for p in Pauses];

def makesubtracks(directory):
	origin=os.path.join(directory,"gpx","origin.gpx");
	points=readpoints(origin);
	assert(points);
	# first the gaps.
	G=findGaps(points);
	P=cuts(points,G);
	assert(P);
	subtracks=list();
	npauses=0;
	for p in P:	
		I=long_pauses(p,findPausingIntervals(p));
		npauses+=len(I);
		subtracks.extend(remove_intervals(p,I));
	Nsum=sum([len(s) for s in subtracks]);
	print(len(G),npauses,Nsum,len(points))
	return subtracks;		

def readtrack(directory):
	print("read",directory)
	if not os.path.exists(os.path.join(directory,"subtracks")):
		S=makesubtracks(directory);
		#print(len(S),"subtracks");

if __name__ == "__main__":
	if len(sys.argv)>1:
		dirname=sys.argv[1];
	else:
		#dirname="/home/julien/projects/tracks/a86483799d2afca78bde6e1662ad8281";
		#dirname="/home/julien/projects/tracks/ba7d58fad784a0270d47cda199971a00"
		dirname="/home/julien/projects/tracks/bb2cc714674ef5fdb0c0423a593e0344"
	readtrack(dirname);

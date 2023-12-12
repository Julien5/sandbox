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

from readtracksutils import *;

def readgpx(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	return gpx;

def writegpx(filename,gpx):
	f=open(filename,'w');	
	f.write(gpx.to_xml());
	f.close();

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

def kmh(ms):
	return ms*3600/1000;

def movement(points,n):
	d=0;
	t=0;
	speed=0;
	if n<0 or n >= len(points)-1:
		return (d,t,speed);
	p1=points[n];
	p2=points[n+1];
	d=distance(p1,p2);
	t=time_seconds(p1,p2);
	if t != 0:
		speed=d/t;
	return (d,t,speed);


def long_pause_interval(points,interval):
	minpause=datetime.timedelta(minutes=120);
	return interval.duration(points)>minpause;

def train_join_condition(points,I1,I2):
	assert(I1.begin<I2.begin);
	p1=points[I1.end];
	p2=points[I2.begin];
	assert(p2.time > p1.time);
	dt=p2.time - p1.time
	return dt<datetime.timedelta(minutes=60);

def moving_train_join_condition(points,I1,I2):
	assert(I1.begin<I2.begin);
	T1=I1.typename;
	T2=I2.typename;
	TT={T1,T2};
	return "train" in TT and "moving" in TT:

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
	maxdd=1000;
	return dt>maxdt or distance(p1,p2)>maxdd;


def apply_intervals(points,intervals):
	ret=list();
	for I in intervals:
		begin=I.begin;
		end=I.end;
		p2=points[begin:end]
		ret.append(p2);
	return ret;



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
		self.distance=0;
		self.moving_seconds=0;
		minspeed=4*1000.0/3600; # m/s
		self.maxspeed=0;
		if interval is None:
			interval = Interval("all",0,N);

		last_speed=None;
		for k in range(interval.begin,interval.end):
			(d,t,speed) = movement(points,k);
			#if t<5 or t>120:
			#	# too short or too long.
			#	continue;
			delta_speed=0;
			if not last_speed is None:
				delta_speed = speed-last_speed;
			#print(f"{str(interval):10s} {k:3d} {d:5.1f}m {t:5.1f}s {kmh(delta_speed):5.1f}kmh {kmh(speed):5.1f}kmh");

			self.distance += d;
			if kmh(delta_speed)<10:
				self.maxspeed=max(self.maxspeed,speed);
			if speed > minspeed:
				self.moving_seconds += t;
			last_speed=speed;	
		self.seconds = time_seconds(points[interval.begin], points[interval.end-1]);
		self.directory = directory;
		self.typename = interval.typename;
		self.duration = points[interval.end-1].time - points[interval.begin].time;
		self.meanspeed = 0;
		self.movingspeed = 0;
		self.gpx = None;
		self.start = interval.begin;
		self.end = interval.end;
		self.startpoint=points[interval.begin];
		self.endpoint=points[interval.end-1];
		self.N =interval.end-interval.begin;
		if self.seconds>0:
			self.meanspeed= self.distance/self.seconds;
		if self.moving_seconds>0:
			self.movingspeed= self.distance/self.moving_seconds;

	def accumulate(self,other):
		self.directory = str();
		assert(self.typename == other.typename);
		self.distance += other.distance;
		self.duration += other.duration;
		self.seconds += other.seconds;
		self.moving_seconds += other.moving_seconds;
		self.meanspeed = 0;
		self.movingspeed= 0;
		#if kmh(other.maxspeed)>80:
			#print("details:");
			#print_statistics(other);
			#readallstatsD(other.directory);
		self.maxspeed=max(self.maxspeed,other.maxspeed);
		self.start = None;
		self.end = None;
		if self.startpoint.time > other.startpoint.time:
			self.startpoint=other.startpoint;
		if self.endpoint.time < other.endpoint.time:
			self.endpoint=other.endpoint;	
		self.endpoint=other.endpoint;
		self.N += other.N;
		if self.seconds>0:
			self.meanspeed = self.distance/self.seconds;
		if self.moving_seconds>0:
			self.movingspeed= self.distance/self.moving_seconds;

def category_speed(msspeed,kmh_threshold):
	if kmh(msspeed)>kmh_threshold:
		return "cycling";
	return "running";

def category(S):
	if S.typename != "moving":
		return "none";
	km=S.distance/1000;
	speed=S.movingspeed;
	if km>40:
		return category_speed(speed,10);
	if km>15:
		return category_speed(speed,15);
	if km>10:
		return category_speed(speed,16);
	return category_speed(speed,17);

def fixutc(time):
	from dateutil import tz
	# METHOD 1: Hardcode zones:
	utc_zone = tz.gettz('UTC')
	local_zone = tz.gettz('Europe/Berlin')


	# Tell the datetime object that it's in UTC time zone since 
	# datetime objects are 'naive' by default
	utc = time.replace(tzinfo=utc_zone)

	# Convert time zone
	return utc.astimezone(local_zone)
	
def print_statistics(S):
	startdate=S.startpoint.time.strftime("%d.%m.%Y (%a)");
	starttime=fixutc(S.startpoint.time).strftime("%H:%M");
	enddate=S.endpoint.time.strftime("%d (%a)");
	endtime=fixutc(S.endpoint.time).strftime("%H:%M");

	cat=category(S);
	print(f"{cat:8s}",end="| ");
	name=S.typename;
	print(f"{name:8s}",end="| ");
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
	maxspeed=3600*S.maxspeed/1000;
	print(f"{speed:4.1f} kmh",end=" |");
	print(f"{mspeed:4.1f} kmh",end=" |");
	print(f"{maxspeed:4.1f} kmh",end=" |");
	print(f"{S.gpx:s}",end=" |");
	print("");


def move_condition(points,n):
	(d,dt,speed)=movement(points,n);
	kmh=3600*speed/1000;
	return kmh>3 and kmh<90;

def trainspeed_condition(points,n):
	(d,dt,speed)=movement(points,n);
	return kmh(speed)>90;

def find_train_intervals(directory,points):
	ret=list();
	
	I0=findIntervals("train",points,trainspeed_condition);
	I_trains=join_intervals(I0,points,train_join_condition);
	ret=union(ret,I_trains);
	
	I_pauses=findIntervals("pause",points,pause_condition);
	k3min=datetime.timedelta(minutes=3);
	interval.duration(points)>k3min;
	I_long_pauses=filter_intervals(Ipauses,points,lambda interval:interval.duration(points)>minpause);
	ret=union(ret,I_long_pauses);

	I_moving=complement_intervals("moving",ret);
	I_trains=join_intervals(ret,points,moving_train_join_condition);
	
	
	subtracks=apply_intervals(points,I_sorted);
	N=len(subtracks);
	for n in range(N):
		interval=I_sorted[n];
		typename=interval.typename;
		track=subtracks[n];
		gpxfilename=os.path.join(directory,"gpx",f"{n:02d}-{interval.typename:s}.gpx");
		writepoints(track,gpxfilename);
		stats=Statistics(directory,points,interval);
		stats.gpx=gpxfilename;
		statsfilename=os.path.join(directory,"gpx",f"{n:02d}-{interval.typename:s}.txt");
		writestats(stats,statsfilename);
		print_statistics(stats);
	

def create_statistics(directory):
	origin=os.path.join(directory,"gpx","origin.gpx");
	points=readpoints(origin);
	assert(points);

	stats=Statistics(directory,points);
	writestats(stats,os.path.join(directory,"gpx","all.txt"));
	find_train_intervals(directory,points);
	return;	
	
	I=[];
	
	Ipauses=findIntervals("tmp",points,pause_condition);
	I.extend(filter_intervals(Ipauses,points,long_pause_interval));

	print("Itrain",len(Itrain));
	I.extend(Itrain);
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
		# if typename != "moving":
		#	# skip
		#	continue;
		begin=interval.begin;
		end=interval.end;
		track=subtracks[n];
		gpxfilename=os.path.join(directory,"gpx",f"{n:02d}-{interval.typename:s}.gpx");
		writepoints(track,gpxfilename);
		stats=Statistics(directory,points,interval);
		#if stats.distance<1000:
		#	# skip
		#	continue;
		stats.gpx=gpxfilename;
		statsfilename=os.path.join(directory,"gpx",f"{n:02d}-{interval.typename:s}.txt");
		s=serialize_stats(stats);
		s2=deserialize_stats(s);
		writestats(stats,statsfilename);
		print_statistics(stats);

from glob import glob
def statsfilesH(dirname):
	return glob(dirname+"/**/*-*.txt", recursive=True);

def statsfilesD(dirname):
	return glob(dirname+"/**/*-*.txt", recursive=True);

def readallstatsD(directory):
	D={};
	for filename in statsfilesD(directory):
		stats=readstats(filename);
		D[stats.startpoint.time]=stats;
	for time in sorted(D.keys()):
		s=D[time];
		print_statistics(s);

def readallstats():
	D={};
	#dirs=glob("/home/julien/projects/tracks/*/", recursive=False);
	dirs=glob("/home/julien/projects/tracks/5ccbb7d88e86ce7c0dfb83e31fd98622/", recursive=False);
	# create statistics if needed
	for n in range(len(dirs)):
		dirname=dirs[n];
		percent=100*n/len(dirs);
		alltxt=os.path.join(dirname,"gpx","all.txt")
		if True or not os.path.exists(alltxt):
			#print(f"{dirname:50s} [{percent:04.1f}%]",end="",flush=True);
			create_statistics(dirname);
			#print("\r",end="",flush="True");

	# gather statistics		
	for dirname in dirs:
		for filename in statsfilesH(dirname):
			stats=readstats(filename);
			D[stats.startpoint.time]=stats;

	acc={};		
	for time in sorted(D.keys()):
		s=D[time];
		if False or (s.typename == "moving" and s.distance>1000):
			if kmh(s.maxspeed)>100:
				print_statistics(s);
			key=category(s);#+s.typename;
			if not key in acc:
				acc[key]=s;
			else:
				acc[key].accumulate(s);

	for key in acc.keys():
		print(key);
		print_statistics(acc[key]);
		
def main():
	readallstats();

if __name__ == "__main__":
	main();	

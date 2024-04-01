#!/usr/bin/env python3

import utils;
import pickle;
import math;

class Statistics:
	def __init__(self,directory,points,interval=None):
		N=len(points);
		self.distance=0;
		self.moving_seconds=0;
		minspeed=3*1000.0/3600;   # m/s
		assert(utils.kmh(minspeed)==3);
		self.maxspeed=0;
		if interval is None:
			interval = utils.Interval("all",0,N);

		last_speed=None;
		# this is slow:
		for k in range(interval.begin,interval.end - 1):
			(d,t,speed) = utils.movement(points,k);
			delta_speed=None;
			if last_speed is not None:
				delta_speed = speed - last_speed;

			self.distance += d;
			if delta_speed is not None and utils.kmh(delta_speed)<5:
				# print(d,t,kmh(last_speed),kmh(delta_speed),kmh(speed));
				self.maxspeed=max(self.maxspeed,speed);
			if speed > minspeed:
				self.moving_seconds += t;
			last_speed=speed;

		p1,p2=points[interval.begin], points[interval.end - 1];
		print("p1.time=",p1.time,"p2.time=",p2.time);
		print(interval,len(points),directory);
		assert(p2.time>p1.time);
		self.seconds = utils.time_seconds(points[interval.begin], points[interval.end - 1]);
		self.directory = directory;
		self.typename = interval.typename;
		self.duration = points[interval.end - 1].time - points[interval.begin].time;
		self.meanspeed = 0;
		self.movingspeed = 0;
		self.gpx = None;
		self.start = interval.begin;
		self.end = interval.end;
		self.startpoint=points[interval.begin];
		self.endpoint=points[interval.end - 1];
		self.N=interval.end-interval.begin;
		if self.seconds>0:
			self.meanspeed=self.distance/self.seconds;
		if self.moving_seconds>0:
			self.movingspeed=self.distance/self.moving_seconds;
		if self.movingspeed==0:
			self.meanspeed=0;
		assert(self.moving_seconds<=self.seconds);
		assert(self.movingspeed>=self.meanspeed);

	def begintime(self):
		return self.startpoint.time;

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

def fr(day):
	D=dict();
	D["Mon"]="lun";
	D["Tue"]="mar";
	D["Wed"]="mer";
	D["Thu"]="jeu";
	D["Fri"]="ven";
	D["Sat"]="sam";
	D["Sun"]="dim";
	return D[day];

def fixutc(time):
	from dateutil import tz
	utc_zone = tz.gettz('UTC')
	local_zone = tz.gettz('Europe/Berlin')
	utc = time.replace(tzinfo=utc_zone)
	return utc.astimezone(local_zone)

def print_statistics_friendly(S):
	startdayfr=fr(S.startpoint.time.strftime("%a"));
	startdate=startdayfr+". "+S.startpoint.time.strftime("%d");
	starttime=fixutc(S.startpoint.time).strftime("%H:%M");
	# enddate=S.endpoint.time.strftime("%d (%a)");
	endtime=fixutc(S.endpoint.time).strftime("%H:%M");

	cat=utils.category(S);
	print(f"{cat:8s}",end="| ");
	print(f"{startdate:8s}",end=" ");
	print(f"{starttime:5s}",end=" - ");
	print(f"{endtime:5s}",end=" |");
	distance=S.distance;
	print(f"{distance/1000:6.1f} km",end=" | ");
	ds=S.seconds;
	hours=math.floor(ds/3600);
	seconds=ds-3600*hours;
	minutes=math.floor(seconds/60);
	seconds=ds-3600*hours-60*minutes;
	print(f"{hours:3d}:{minutes:02d}:{int(seconds):02d}",end=" | ");
	avespeed=utils.kmh(S.meanspeed);
	movspeed=utils.kmh(S.movingspeed);
	maxspeed=utils.kmh(S.maxspeed);
	print(f"{avespeed:4.1f} kmh",end=" |");
	print(f"{movspeed:4.1f} kmh",end=" |");
	print(f"{maxspeed:4.1f} kmh",end=" |");
	print("");

def print_statistics(S):
	startdate=S.startpoint.time.strftime("%d.%m.%Y (%a)");
	starttime=fixutc(S.startpoint.time).strftime("%H:%M");
	# enddate=S.endpoint.time.strftime("%d (%a)");
	endtime=fixutc(S.endpoint.time).strftime("%H:%M");

	cat=utils.category(S);
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
	avespeed=utils.kmh(S.meanspeed);
	movspeed=utils.kmh(S.movingspeed);
	maxspeed=utils.kmh(S.maxspeed);
	print(f"{avespeed:4.1f} kmh",end=" |");
	print(f"{movspeed:4.1f} kmh",end=" |");
	print(f"{maxspeed:4.1f} kmh",end=" |");
	print(f"{S.gpx:s}",end=" |");
	print("");

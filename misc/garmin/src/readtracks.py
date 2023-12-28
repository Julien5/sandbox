#!/usr/bin/env python3

import gpxpy
import gpxpy.gpx

import os;
import datetime;
import math;

import pickle;
import chain;

import readtracksutils;

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
	return readtracksutils.Point(p.latitude,p.longitude,p.elevation,p.time);

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

def duration(points,interval):
	return points[interval.end].time - points[interval.begin].time;

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
	return "train" in TT and "moving" in TT;

def pause_condition(m):
	(d,dt,speed)=m;
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
	for interval in intervals:
		begin=interval.begin;
		end=interval.end;
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
		minspeed=3*1000.0/3600;   # m/s
		assert(kmh(minspeed)==3);
		self.maxspeed=0;
		if interval is None:
			interval = readtracksutils.Interval("all",0,N);

		last_speed=None;
		# this is slow:
		for k in range(interval.begin,interval.end - 1):
			(d,t,speed) = movement(points,k);
			delta_speed=None;
			if last_speed is not None:
				delta_speed = speed - last_speed;

			self.distance += d;
			if delta_speed is not None and kmh(delta_speed)<5:
				# print(d,t,kmh(last_speed),kmh(delta_speed),kmh(speed));
				self.maxspeed=max(self.maxspeed,speed);
			if speed > minspeed:
				self.moving_seconds += t;
			last_speed=speed;

		self.seconds = time_seconds(points[interval.begin], points[interval.end - 1]);
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
	# enddate=S.endpoint.time.strftime("%d (%a)");
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
	avespeed=kmh(S.meanspeed);
	movspeed=kmh(S.movingspeed);
	maxspeed=kmh(S.maxspeed);
	print(f"{avespeed:4.1f} kmh",end=" |");
	print(f"{movspeed:4.1f} kmh",end=" |");
	print(f"{maxspeed:4.1f} kmh",end=" |");
	print(f"{S.gpx:s}",end=" |");
	print("");

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
	
def print_statistics_friendly(S):
	startdayfr=fr(S.startpoint.time.strftime("%a"));
	startdate=startdayfr+". "+S.startpoint.time.strftime("%d");
	starttime=fixutc(S.startpoint.time).strftime("%H:%M");
	# enddate=S.endpoint.time.strftime("%d (%a)");
	endtime=fixutc(S.endpoint.time).strftime("%H:%M");

	cat=category(S);
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
	avespeed=kmh(S.meanspeed);
	movspeed=kmh(S.movingspeed);
	maxspeed=kmh(S.maxspeed);
	print(f"{avespeed:4.1f} kmh",end=" |");
	print(f"{movspeed:4.1f} kmh",end=" |");
	print(f"{maxspeed:4.1f} kmh",end=" |");
	print("");

def move_condition(points,n):
	(d,dt,speed)=movement(points,n);
	kmh=3600*speed/1000;
	return kmh>3 and kmh<90;

def trainspeed_condition(m):
	(d,dt,speed)=m;
	return kmh(speed)>90;

def annotation_function(points,n):
	if gap_condition(points,n):
		return "gap";
	m=movement(points,n);
	if pause_condition(m):
		return "pause";
	if trainspeed_condition(m):
		return "train";
	return "moving";

def train_start_condition(I):
	return I.typename == "train";

class WithPoints:
	def __init__(self,points):
		self.points=points;
		
	def train_join_condition(self,I1,I2):
		assert(I1.typename=="train");
		if I2.typename != "train":
			return False;
		assert(I1.begin<=I1.end);
		assert(I1.end<=I2.begin);
		p1=self.points[I1.end];
		p2=self.points[I2.begin];
		assert(p2.time >= p1.time);
		dt=p2.time - p1.time
		return dt<datetime.timedelta(minutes=60);

	def moving_join_condition(self,I1,I2):
		names={I1.typename,I2.typename}
		#print(I1,I2);
		if "train" in names:
			return False;
		assert("moving" in names);
		if len(names)==1:
			# moving, moving
			return True;
		P=[I for I in [I1,I2] if I.typename=="pause"];
		assert(len(P)==1);
		pause=P[0];
		minpause=datetime.timedelta(minutes=60);
		return duration(self.points,pause)<minpause;

	def pause_join_condition(self,I1,I2):
		names={I1.typename,I2.typename}
		#print(I1,I2);
		if "train" in names:
			return False;
		assert("pause" in names);
		if len(names)==1:
			# pause, pause
			return True;
		P=[I for I in [I1,I2] if I.typename=="moving"];
		assert(len(P)==1);
		moving=P[0];
		minmove=datetime.timedelta(minutes=5);
		return duration(self.points,moving)<minmove;
	
def print_subtracks(directory,gapk,points,J,write_on_disk=False):
	subtracks=apply_intervals(points,J);
	prefix=f"GAP{gapk:02d}-";
	if write_on_disk:
		stats=Statistics(directory,points,readtracksutils.Interval("all",0,len(points)));
		statsfilename=os.path.join(directory,"gpx",prefix+"all.txt");
		writestats(stats,statsfilename);
	N=len(subtracks);
	for n in range(N):
		interval=J[n];
		# typename=interval.typename;
		track=subtracks[n];
		gpxfilename=os.path.join(directory,"gpx",prefix+f"{n:02d}-{interval.typename:s}.gpx");
		if write_on_disk:
			writepoints(track,gpxfilename);
		stats=Statistics(directory,points,interval);
		stats.gpx=gpxfilename;
		statsfilename=os.path.join(directory,"gpx",prefix+f"{n:02d}-{interval.typename:s}.txt");
		if write_on_disk:
			writestats(stats,statsfilename);
		if not write_on_disk:
			print_statistics(stats);

def moving_start_condition(I):
	return I.typename == "moving";

def pause_start_condition(I):
	return I.typename == "pause";

def debug(banner,directory,k,points,J):
	#print(banner);
	#print_subtracks(directory,k,points,J);
	return;

def formatdelta(dt):
	t0=datetime.datetime(1970,1,1);
	t1=t0+dt;
	return t1.strftime("%S.%f");

class Timer:
	def __init__(self):
		self.t0=datetime.datetime.now();

	def elapsed(self,name):
		t1=datetime.datetime.now();
		dt=t1-self.t0;
		print(f" [{name:10s} {formatdelta(dt):s}]");
		self.t0=datetime.datetime.now();

def process_intervals(directory,k,points,J):
	timer=Timer();
	debug("****** raw *****",directory,k,points,J);
	withPoints=WithPoints(points);
	J=readtracksutils.join_far_intervals(J,train_start_condition,withPoints.train_join_condition)
	debug("****** far *****",directory,k,points,J);
	
	J=readtracksutils.join_close_intervals(J,train_start_condition,lambda I1,I2: True);
	debug("****** close *****",directory,k,points,J);

	J=readtracksutils.join_close_intervals(J,moving_start_condition,withPoints.moving_join_condition);
	debug("****** long pause *****",directory,k,points,J);

	J=readtracksutils.join_close_intervals(J,pause_start_condition,withPoints.pause_join_condition);
	debug("****** long moving *****",directory,k,points,J);
	timer.elapsed("process");

	print_subtracks(directory,k,points,J,write_on_disk=True);
	timer.elapsed("print");

def create_statistics(directory):
	origin=os.path.join(directory,"gpx","origin.gpx");
	timer=Timer();
	points=readpoints(origin);
	timer.elapsed("read");
	assert(points);

	J=readtracksutils.annotate(points,annotation_function);
	timer.elapsed("annotate");
	
	R=readtracksutils.split(J,lambda interval : interval.typename=="gap");
	timer.elapsed("split");
	
	for k in range(len(R)):
		J=R[k];
		process_intervals(directory,k,points,J);

from glob import glob
def statsfilesH(dirname):
	return glob(dirname+"/**/GAP*-*-*.txt", recursive=True);

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
	dirs=glob("/home/julien/projects/tracks/*/", recursive=False);
	#dirs=glob("test/H/*/", recursive=False);
	#dirs=glob("/home/julien/projects/tracks/5ccbb7d88e86ce7c0dfb83e31fd98622/", recursive=False);
	# create statistics if needed
	for n in range(len(dirs)):
		dirname=dirs[n];
		percent=100*n/len(dirs);
		alltxt=os.path.join(dirname,"gpx","GAP00-all.txt")
		#doit=False;
		doit=False;
		if doit or not os.path.exists(alltxt):
			create_statistics(dirname);
			print(f"{dirname:50s} [{percent:04.1f}%]",end="");
			print("\n",end="",flush="True");

	# gather statistics		
	for dirname in dirs:
		for filename in statsfilesH(dirname):
			stats=readstats(filename);
			D[stats.startpoint.time]=stats;
			#print(stats.startpoint.time,filename,stats.typename,stats.distance);

	last_month=None;		
	acc={};
	T=sorted(D.keys())
	Tcycling=list();
	for k in range(len(T)):
		time=T[k];
		month=time.strftime("%m.%Y");
		month_changed = month != last_month;

		if month_changed:
			print("-"*10)
			for key in sorted(acc.keys()):
				print_statistics_friendly(acc[key]);
			acc={};	
			print();
			print(month);

		s=D[time];
		if s.typename == "moving" and s.distance>1000:
			print_statistics_friendly(s);
			if category(s) == "running":
				print("gpx:",s.gpx);
			if category(s) == "cycling":
				Tcycling.append(s);
			key=category(s);
			if key not in acc:
				acc[key]=s;
			else:
				acc[key].accumulate(s);

		last_month=month;

	if acc:
		print("-"*10)
		for key in sorted(acc.keys()):
			print_statistics_friendly(acc[key]);

	chain.chain_distances(Tcycling);	


def main():
	readallstats();


if __name__ == "__main__":
	main();	

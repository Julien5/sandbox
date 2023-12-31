#!/usr/bin/env python3

import gpxpy
import gpxpy.gpx

import os;
import datetime;
import statistics;

import utils;

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
	return utils.Point(p.latitude,p.longitude,p.elevation,p.time);

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
	return dt>maxdt or utils.distance(p1,p2)>maxdd;

def apply_intervals(points,intervals):
	ret=list();
	for interval in intervals:
		begin=interval.begin;
		end=interval.end;
		p2=points[begin:end]
		ret.append(p2);
	return ret;

def move_condition(points,n):
	(d,dt,speed)=utils.movement(points,n);
	return utils.kmh(speed)>3 and utils.kmh(speed)<90;

def trainspeed_condition(m):
	(d,dt,speed)=m;
	return utils.kmh(speed)>90;

def annotation_function(points,n):
	if gap_condition(points,n):
		return "gap";
	m=utils.movement(points,n);
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
		return utils.duration(self.points,pause)<minpause;

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
		return utils.duration(self.points,moving)<minmove;
	
def print_subtracks(directory,gapk,points,J,write_on_disk=False):
	subtracks=apply_intervals(points,J);
	prefix=f"GAP{gapk:02d}-";
	if write_on_disk:
		stats=statistics.Statistics(directory,points,utils.Interval("all",0,len(points)));
		statsfilename=os.path.join(directory,"gpx",prefix+"all.txt");
		statistics.writestats(stats,statsfilename);
	N=len(subtracks);
	for n in range(N):
		interval=J[n];
		# typename=interval.typename;
		track=subtracks[n];
		gpxfilename=os.path.join(directory,"gpx",prefix+f"{n:02d}-{interval.typename:s}.gpx");
		if write_on_disk:
			writepoints(track,gpxfilename);
		stats=statistics.Statistics(directory,points,interval);
		stats.gpx=gpxfilename;
		statsfilename=os.path.join(directory,"gpx",prefix+f"{n:02d}-{interval.typename:s}.txt");
		if write_on_disk:
			statistics.writestats(stats,statsfilename);
		if not write_on_disk:
			statistics.print_statistics(stats);

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
	J=utils.join_far_intervals(J,train_start_condition,withPoints.train_join_condition)
	debug("****** far *****",directory,k,points,J);
	
	J=utils.join_close_intervals(J,train_start_condition,lambda I1,I2: True);
	debug("****** close *****",directory,k,points,J);

	J=utils.join_close_intervals(J,moving_start_condition,withPoints.moving_join_condition);
	debug("****** long pause *****",directory,k,points,J);

	J=utils.join_close_intervals(J,pause_start_condition,withPoints.pause_join_condition);
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

	J=utils.annotate(points,annotation_function);
	timer.elapsed("annotate");
	
	R=utils.split(J,lambda interval : interval.typename=="gap");
	timer.elapsed("split");
	
	for k in range(len(R)):
		J=R[k];
		process_intervals(directory,k,points,J);

def main():
	print("fixme");


if __name__ == "__main__":
	main();	



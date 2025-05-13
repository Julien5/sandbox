#!/usr/bin/env python3

import os
import datetime
import copy;

import readtracks;
import utils;
import statistics;

def filter_intervals(intervals,points,function):
	ret=list();
	for interval in intervals:
		if function(points,interval):
			ret.append(interval);
	return ret;

def gap_condition(I):
	return I.typename=="gap";

def split(J,condition):
	R=list();
	packet=list();
	for k in range(len(J)):
		if condition(J[k]) or k==len(J)-1:
			R.append(packet);
			packet=list();
		else:
			packet.append(J[k]);
	return R;

def next_joinable_interval(intervals,startindex,join_condition):
	N=len(intervals);
	ref=intervals[startindex];
	for k in range(startindex+1,N):
		I2=intervals[k];
		if join_condition(ref,I2):
			return k;
	return None;

def next_interval(intervals,startindex,condition):
	N=len(intervals);
	for k in range(startindex,N):
		I=intervals[k];
		if condition(I):
			return k;
	return None;

def join_far_intervals(intervals,start_condition,join_condition):
	N=len(intervals);
	k=next_interval(intervals,0,start_condition);
	if k is None:
		return copy.deepcopy(intervals);
	assert(k<N);
	ret=copy.deepcopy(intervals[:k+1]);
	while True:
		assert(start_condition(ret[-1]));
		l=next_joinable_interval(intervals,k,join_condition);
		if l is None:
			break;
		ret[-1].join(intervals[l]);
		k=l
	if k+1<N:
		ret.extend(intervals[k+1:]);
	return ret;

def join_close_intervals(intervals,start_condition,join_condition):
	k=-1;
	ret=copy.deepcopy(intervals);
	while True:
		N=len(ret);
		k=next_interval(ret,k+1,start_condition);
		if k is None:
			break;
		if k<(N-1):
			next=ret[k+1];
			if join_condition(ret[k],next):
				ret[k].join(next);
				del ret[k+1];
		if k>0:
			prev=ret[k-1];
			if join_condition(prev,ret[k]):
				ret[k].join(prev);
				del ret[k-1];
				k=k-1;
	return ret;

def test_join_far_intervals():
	intervals=list();
	intervals.append(utils.Interval("M", 1, 4));
	intervals.append(utils.Interval("T", 4,10));
	intervals.append(utils.Interval("M",10,11));
	intervals.append(utils.Interval("T",11,13));
	intervals.append(utils.Interval("P",13,14));
	intervals.append(utils.Interval("P",21,23));
	intervals.append(utils.Interval("T",51,53));
	intervals.append(utils.Interval("P",61,63));
	for i in intervals:
		print(i)
	print("=>");
	J=join_far_intervals(intervals, lambda I: I.typename == "T", lambda I1,I2: I1.typename=="T" and I2.typename=="T" and I2.begin-I1.end<10);
	for i in intervals:
		print(i)
	print("=>");	
	for i in J:
		print(i);

def join_intervals(intervals,join_condition):
	ret=list();
	N=len(intervals);
	assert(intervals);
	ret=[intervals[0]];
	k=1;
	for k in range(1,N):
		tail=ret[-1]
		nexti=intervals[k];
		if join_condition(tail,nexti):
			ret[-1].join(nexti);
		else:
			ret.append(nexti);
		k=k+1;
	return ret;

def test_join_condition(I1,I2):
	return I2.begin - I1.end < 3;

def test_join_intervals():
	I=list();
	I.append(utils.Interval("a",1,3));
	I.append(utils.Interval("a",5,6))
	I.append(utils.Interval("b",15,17))
	I.append(utils.Interval("b",19,21))
	I.append(utils.Interval("b",22,24))
	I.append(utils.Interval("c",35,37))
	R=join_intervals(I,test_join_condition);
	for r in I:
		print(r)
	print("=>");
	for r in R:
		print(r)

def remove_spurious_train_interval(intervals):
	ret=[];
	for interval in intervals:
		# not good yet:
		# test on /home/julien/projects/tracks/5423206c7a9a35268c6d4fb9901cb1eb
		# /home/julien/tracks/2025.05.11/Track_10-MAY-25 033018.gpx
		if interval.typename == "train" and interval.end-interval.begin<2: # length=1
			if ret:
				ret[-1].end=interval.end;
		else:
			ret.append(interval);
	return ret;

def annotate(points,annotation_function):
	assert(points);
	intervals=[utils.Interval(annotation_function(points,0),0,None)];
	N=len(points);
	A=[annotation_function(points,n) for n in range(len(points))];

	for n in range(1,len(A)-1):
		if A[n] == "train":
			if A[n-1] != "train" and A[n+1] != "train":
				pass;#A[n]=A[n-1];
			
	for n in range(1,N):
		annotation=A[n];#annotation_function(points,n);
		change=intervals[-1].typename != A[n];
		if not change:
			continue;
		# close the current one
		intervals[-1].end=n;
		# open new one 
		intervals.append(utils.Interval(annotation,n,None));
	intervals[-1].end=N;
	# remove None
	# intervals=[intervals[k] for k in range(len(intervals)) if not intervals[k].typename is None];
	return remove_spurious_train_interval(intervals);

def test_annotation_function(points,n):
	if points[n].elevation is None:
		return None;
	if points[n].elevation<1:
		return "low";
	if points[n].elevation>2:
		return "high";
	return "middle";

def test_annotate():
	I=list();
	points=list();
	points.append(utils.Point(0,0,0.1,0));
	points.append(utils.Point(0,0,0.2,0));
	points.append(utils.Point(0,0,0.2,0));
	points.append(utils.Point(0,0,0.2,0));
	points.append(utils.Point(0,0,1.1,0));
	points.append(utils.Point(0,0,2.3,0));
	points.append(utils.Point(0,0,None,0));
	points.append(utils.Point(0,0,None,0));
	points.append(utils.Point(0,0,2.3,0));
	points.append(utils.Point(0,0,2.3,0));
	J=annotate(points,test_annotation_function);
	assert(J);
	for I in J:
		print(I);

def test():
	test_annotate();
	print("*"*20);
	test_join_intervals();
	print("*"*20);
	test_join_far_intervals();
	print("*"*20);

if __name__ == "__main__":
	test();	

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
		#t1=points[n].time;
		#t2=points[n+1].time;
		#dist=points[n].dist(points[n+1]);
		#print("t1=",t1);
		#print("t2=",t2)
		#print("d=",dist);
		#print("speed=",utils.kmh(dist/(t2-t1).seconds));
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
			readtracks.writepoints(track,gpxfilename);
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
	J=join_far_intervals(J,train_start_condition,withPoints.train_join_condition)
	debug("****** far *****",directory,k,points,J);
	
	J=join_close_intervals(J,train_start_condition,lambda I1,I2: True);
	debug("****** close *****",directory,k,points,J);

	J=join_close_intervals(J,moving_start_condition,withPoints.moving_join_condition);
	debug("****** long pause *****",directory,k,points,J);

	J=join_close_intervals(J,pause_start_condition,withPoints.pause_join_condition);
	debug("****** long moving *****",directory,k,points,J);
	timer.elapsed("process");

	print_subtracks(directory,k,points,J,write_on_disk=True);
	timer.elapsed("print");

def create_statistics(directory):
	origin=os.path.join(directory,"gpx","origin.gpx");
	timer=Timer();
	name,points,creator=readtracks.readpoints(origin);
	timer.elapsed("read");
	assert(points);
	J=annotate(points,annotation_function);
	timer.elapsed("annotate");
	
	R=split(J,lambda interval : interval.typename=="gap");
	timer.elapsed("split");
	
	for k in range(len(R)):
		J=R[k];
		process_intervals(directory,k,points,J);

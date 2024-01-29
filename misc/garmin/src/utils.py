#!/usr/bin/env python3

import copy

class Point:
	def __init__(self,la,lo,ele,time):
		self.latitude=la;
		self.longitude=lo;
		self.elevation=ele;
		self.time=time;

class Interval:
	def __init__(self,typename,begin,end):
		self.begin=begin;
		self.end=end;
		self.typename=typename;

	def contains(self,n):
		return self.begin<=n and n<self.end;

	def __str__(self):
		typename=self.typename;
		if typename is None:
			typename=str(None);
		return f"{typename:10s}:{self.begin:d}-{self.end:d}";

	def join(self,other):
		b=min(self.begin,other.begin);
		e=max(self.end,other.end);
		self.begin=b;
		self.end=e;

class StartTimeComparator:
	def __init__(self,points):
		self.points=points;

	def key(self,interval):
		return self.points[interval.begin].time;


def kmh(ms):
	return ms*3600/1000;

def distance(p1,p2):
	import geopy.distance
	pp1=(p1.latitude,p1.longitude);
	pp2=(p2.latitude,p2.longitude);
	return 1000*geopy.distance.geodesic(pp1,pp2).km;

def time_seconds(p1,p2):
	# print(p1.time);
	# print(p2.time);
	return (p2.time-p1.time).total_seconds();

def duration(points,interval):
	return points[interval.end].time - points[interval.begin].time;

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
	intervals.append(Interval("M", 1, 4));
	intervals.append(Interval("T", 4,10));
	intervals.append(Interval("M",10,11));
	intervals.append(Interval("T",11,13));
	intervals.append(Interval("P",13,14));
	intervals.append(Interval("P",21,23));
	intervals.append(Interval("T",51,53));
	intervals.append(Interval("P",61,63));
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
	I.append(Interval("a",1,3));
	I.append(Interval("a",5,6))
	I.append(Interval("b",15,17))
	I.append(Interval("b",19,21))
	I.append(Interval("b",22,24))
	I.append(Interval("c",35,37))
	R=join_intervals(I,test_join_condition);
	for r in I:
		print(r)
	print("=>");
	for r in R:
		print(r)

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

def annotate(points,annotation_function):
	assert(points);
	intervals=[Interval(annotation_function(points,0),0,None)];
	N=len(points);
	for n in range(1,N):
		annotation=annotation_function(points,n);
		change=intervals[-1].typename != annotation;
		if not change:
			continue;
		# close the current one
		intervals[-1].end=n;
		# open new one 
		intervals.append(Interval(annotation,n,None));
	intervals[-1].end=N;
	# remove None
	# intervals=[intervals[k] for k in range(len(intervals)) if not intervals[k].typename is None];
	return intervals;

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
	points.append(Point(0,0,0.1,0));
	points.append(Point(0,0,0.2,0));
	points.append(Point(0,0,0.2,0));
	points.append(Point(0,0,0.2,0));
	points.append(Point(0,0,1.1,0));
	points.append(Point(0,0,2.3,0));
	points.append(Point(0,0,None,0));
	points.append(Point(0,0,None,0));
	points.append(Point(0,0,2.3,0));
	points.append(Point(0,0,2.3,0));
	J=annotate(points,test_annotation_function);
	assert(J);
	for I in J:
		print(I);

def main():
	test_annotate();
	print("*"*20);
	test_join_intervals();
	print("*"*20);
	test_join_far_intervals();
	print("*"*20);

if __name__ == "__main__":
	main();	

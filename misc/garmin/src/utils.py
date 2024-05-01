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


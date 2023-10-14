#!/usr/bin/env python3

import os;
import sys;
import datetime;
import math;

def print_stats(track):
	if not track.points():
		print("empty");
		return;
	points=track.points();
	startdate=(track.begintime()+datetime.timedelta(hours=2)).strftime("%d.%m.%Y (%a)");
	starttime=(track.begintime()+datetime.timedelta(hours=2)).strftime("%H:%M");
	
	print(f"{startdate:8s}",end=" ");
	print(f"{starttime:5s}",end=" | ");
	
	print(f"{track.distance()/1000:5.1f} km",end=" | ");
	ds=track.duration().total_seconds();
	hours=math.floor(ds/3600);
	seconds=ds-3600*hours;
	minutes=math.floor(seconds/60);
	print(f"{hours:02d}:{minutes:02d}",end=" | ");
	speed=3600*track.speed()/1000;
	mspeed=3600*track.moving_speed()/1000;
	print(f"{speed:4.1f} kmh",end=" |");
	print(f"{mspeed:4.1f} kmh",end=" |");	
	#print(f"{track.name():s}",end=" |");
	print("");

def print_csv(track):
	ret=str();
	points=track.points();
	startdate=(points[0].time()+datetime.timedelta(hours=2)).strftime("%d.%m.%Y");
	starttime=(points[0].time()+datetime.timedelta(hours=2)).strftime("%H:%M");
	ret=ret+"start;";
	ret=ret+f"{startdate:8s};";
	ret=ret+f"{starttime:5s};";
	ret=ret+f"{track.distance()/1000:5.1f};";
	ret=ret+"km;";	
	ds=track.duration().total_seconds();
	hours=math.floor(ds/3600);
	seconds=ds-3600*hours;
	minutes=math.floor(seconds/60);
	ret=ret+f"{hours:02d}:{minutes:02d};";
	ret=ret+"hh:mm;";	
	speed=0;
	if track.speed():
		speed=3600*track.speed()/1000;
	ret=ret+f"{speed:4.1f};"
	ret=ret+"kmh;";	
	ret=ret+f"{track.name():s};"
	return ret;

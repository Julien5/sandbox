#!/usr/bin/env python3

import os;
import sys;
import datetime;
import math;

def print_stats(track):
	if not track.points():
		print("empty");
		return;
	if track.distance()>500000:
		print(track.string())	
		assert(False);
		
	points=track.points();
	startdate=(points[0].time()+datetime.timedelta(hours=2)).strftime("%d.%m.%Y");
	starttime=(points[0].time()+datetime.timedelta(hours=2)).strftime("%H:%M");
	
	print(f"{startdate:8s}",end=" ");
	print(f"{starttime:5s}",end=" | ");
	
	print(f"{track.distance()/1000:5.1f} km",end=" | ");
	ds=track.duration().total_seconds();
	hours=math.floor(ds/3600);
	seconds=ds-3600*hours;
	minutes=math.floor(seconds/60);
	print(f"{hours:02d}:{minutes:02d}",end=" | ");
	speed=0;
	if track.speed():
		speed=3600*track.speed()/1000;
	print(f"{speed:4.1f} kmh",end=" |");
	print(f"{track.name():s}",end=" |");
	print("");


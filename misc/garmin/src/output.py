#!/usr/bin/env python3

import os;
import sys;
import datetime;
import math;
import readgpx2;

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

def plot_speed(track):
	points=track.points();
	data=[];
	assert(points);
	starttime=points[0].time();
	for n in range(len(points)-1):
		p0=points[n];
		p1=points[n+1];
		d=p0.distance(p1);
		dt=p1.time()-p0.time();
		if dt.total_seconds()==0:
			print(track.name());
			print(p0.string());
			print(p1.string());
			assert(0);
		mps=d/dt.total_seconds();
		kmh=3600*mps/1000;
		t=p0.time();
		data.append((t,kmh));
	
	content=str();
	for n in range(len(data)):
		(t,v)=data[n];
		time=readgpx2.fixUTC(t).strftime("%H:%M");
		secs=(t-starttime).total_seconds();
		content+=f"{time:s}\t{round(secs):d}\t{v:5.2f}\n";
	

	name=track.name();
	dt=name.split("/");
	assert(len(dt)==5);
	dirname="/".join(dt[1:3]);
	datafile=os.path.join("/tmp/plots/",dirname,"speed.data");
	os.makedirs(os.path.dirname(datafile),exist_ok=True);
	open(datafile,"w").write(content);

	M=readgpx2.meta(track);
	tmpl=open("speed.tmpl","r").read();
	km=M["distance"]/1000;
	time=(datetime.datetime(2000,1,1)+(M["end"]-M["start"])).strftime("%H:%M");
	start=M["start"].strftime("%d.%m.%y - %H:%M");
	
	tmpl=tmpl.replace("{km}",f"{km:1.1f}");
	tmpl=tmpl.replace("{time}",f"{time:s}");
	tmpl=tmpl.replace("{name}",f"{name:s}");
	tmpl=tmpl.replace("{start}",f"{start:s}");
	tmpl=tmpl.replace("{datafile}",f"{datafile:s}");
	pngfile=os.path.join("/tmp/plots/images",track.category(),dirname.replace("/","-")+".png");
	os.makedirs(os.path.dirname(pngfile),exist_ok=True);
	tmpl=tmpl.replace("{pngfile}",f"{pngfile:s}");

	gnuplotfile=os.path.join("/tmp/plots/",dirname,"speed.gnuplot");
	open(gnuplotfile,"w").write(tmpl);
	

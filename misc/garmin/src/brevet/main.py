#!/usr/bin/env python3

import gpxpy;
import geopy.distance;
import datetime;
import math;
import sys;
import os;
import argparse;

sys.path.append(os.path.join(os.path.dirname(__file__),".."));

import elevation;
import readtracks;
import utils;
import finder;
import automatic;
from richwaypoint import RichWaypoint;

import output;

def is_automatic(w):
	if w.type is None:
		return False;
	if w.type == "A":
		return True;
	return False;

def read_control_waypoints(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	K=list();
	A=list();
	for w in gpx.waypoints:
		rw=RichWaypoint(utils.Point(w.latitude,w.longitude,w.elevation));
		rw.name=w.name;
		if w.description:
			rw.description=w.description.replace("’","'");
		if w.name:
			rw.name=w.name.strip();
		if not rw.name:
			rw.name="";
		if not w.type is None:
			assert(w.type == "K" or w.type == "A");
			rw.type=w.type;
		if is_automatic(w):
			A.append(rw);
		else:
			K.append(rw);
	return K,A;

def time_as_delta(hours):
	tddays=math.floor(hours/24);
	tdseconds=hours*3600 - tddays*24*3600;
	return datetime.timedelta(days=tddays,seconds=math.floor(tdseconds));

def timehours_to(distance):
	km=distance/1000;
	return km/10;


def waypoint_time(total_hours,start=None):
	delta=time_as_delta(total_hours);
	if start:
		return start+delta;
	secs=delta.total_seconds();
	hours=int(math.floor(secs/3600));
	rsecs = secs - 3600*hours;
	minutes=int(math.floor(rsecs/60));
	return datetime.datetime(2024, 1, 1, hour=hours, minute=minutes);

def fix_summer_winter_time(summer):
	hour=datetime.timedelta(days=0,seconds=3600);
	start_winter=datetime.datetime(summer.year, 10, 29, hour=2, minute=0);
	if summer>start_winter:
		return summer-hour;
	return summer;

def control_waypoint_name(time,waypoint,counter):
	time_str=fix_summer_winter_time(time).strftime("%d-%H:%M");
	time_str=time_str[1:];
	short=waypoint.name;
	if not short:
		short=f"{counter:2d}";
	return f"{short[0:3]:3s}-{time_str:s}"

def toGPXWaypoint(rpoint):
	w = gpxpy.gpx.GPXWaypoint()
	w.latitude=rpoint.point.latitude;
	w.longitude=rpoint.point.longitude;
	w.elevation=rpoint.point.elevation;
	w.name=rpoint.name;
	w.description=rpoint.description;
	w.symbol = "Flag, Blue";
	w.type=rpoint.type;
	return w;

def toGPXSegment(points):
	assert(points);
	w = gpxpy.gpx.GPXTrackSegment()
	for p in points:
		g=gpxpy.gpx.GPXTrackPoint();
		g.latitude=p.latitude;
		g.longitude=p.longitude;
		g.elevation=p.elevation;
		g.time=p.time;
		w.points.append(g);
	return w;

def project_waypoints(richpoints,finder):
	D=dict();
	for richpoint in richpoints:
		distance=finder.find_distance(richpoint.point);

		# we project all points, even controls,
		# because we want exact points
		pr=finder.project(richpoint.point);
		d=utils.distance(pr,richpoint.point);
		print(f"{richpoint.name:20s} {d:5.1f}");
		richpoint.point=pr;

		if distance in D:
			if richpoint.name == D[distance].name:
				continue;
			new=richpoint;
			old=D[distance];
			print("two waypoints with the same time:",time);
			print("new:",new.name,"-",new.description);
			print("old:",old.name,"-",old.description);
			assert(not distance in D);
		richpoint.distance=distance;
		D[distance]=richpoint;
	return D;

def label_waypoints(richpoints,start,track):
	x,y=elevation.load(track);
	D=dict();
	K=set();
	A=set();

	# as list
	R=list();
	for d in sorted(richpoints.keys()):
		R.append(richpoints[d]);

	D=dict();	
	for k in range(len(R)):
		richpoint=R[k];
		prefix=None;
		if richpoint.isControlPoint():
			K.add(richpoint);
			prefix=f"K{len(K):d}";
		else:
			A.add(richpoint);
			prefix=f"A{len(A)%10:d}";
			
		Wprev=None;
		if k>0:
			Wprev=R[k-1]
		
		total_hours=timehours_to(richpoint.distance);
		time=waypoint_time(total_hours,start);
		time_str=fix_summer_winter_time(time).strftime("%H:%M");

		if not Wprev is None:
			dx,dy=automatic.slope(x,y,Wprev,richpoint);
			richpoint.dplus=dy;
			richpoint.xdplus=dx;
			if dx>0:
				richpoint.slope=100*dy/dx;
		# save the original name in the description
		richpoint.description=richpoint.name;
		slope_str="  ";
		if not (richpoint.slope is None):
			slope_str=f"{richpoint.slope:2.0f}"
		richpoint.name=f"{prefix:s}-{slope_str:s}-{time_str:s}";
		print(f"{richpoint.name:s} at {richpoint.distance/1000:3.0f}");
		richpoint.time=time;
		
		D[richpoint.distance]=richpoint;
	return D;



def makegpx(track,waypoints,name,filename):
	global arguments;
	print("remove time and elevation before exporting to gpx");
	segment = toGPXSegment(track);
	if arguments.flat:
		for p in segment.walk(True):
			p.elevation=None;
			p.time=None;
		
	gpx = gpxpy.gpx.GPX();

	gpx_track = gpxpy.gpx.GPXTrack()
	gpx_track.name = name;
	gpx.tracks.append(gpx_track)
	gpx_track.segments.append(segment)
	
	L=[];
	for distance in sorted(waypoints.keys()):
		time=waypoints[distance].time;
		# print(f"{waypoint_string(w):s};{distance/1000:5.1f};{total_hours:3.1f}");
		L.append(toGPXWaypoint(waypoints[distance]));
	gpx.waypoints=L;

	print("generate",filename);
	open(filename,'w').write(gpx.to_xml());

def value(rw):
	if rw.isControlPoint():
		return 10000; # "infinity"
	assert(rw.point.elevation);
	return rw.point.elevation;

def sort_waypoints(W):
	return sorted(W, key=lambda rw: value(rw), reverse=True)

def closest(W,w0):
	assert(len(W)>=2);
	return sorted(W.values(), key=lambda rw: abs(rw.distance-w0.distance))[1];

def filter_waypoints(W):
	if not W:
		return W;
	D=list(W.keys());
	for d in D:
		if not d in W:
			continue;
		w=W[d];
		c=closest(W,w);
		d=abs(c.distance-w.distance);
		if d<4000:
			Sloc=sort_waypoints([w,c]);
			d_hide=Sloc[-1].distance;
			print(f"hide {Sloc[-1].name:s} because it is too close to {Sloc[0].name:s} (d={d:04.1f}m)");
			W[d_hide].label_on_profile=False;
			if d<2000:
				del W[d_hide];
	return W;

arguments=None;
def parse_arguments():
	global arguments;
	parser = argparse.ArgumentParser();
	parser.add_argument('filename') 
	parser.add_argument('-s', '--starttime', help="format: 2024-12-28-08:00:05");
	# flat: default = false
	parser.add_argument('-f', '--flat', action="store_true", help="removes time and elevation data")
	parser.add_argument('-o', '--output', help="output filename")
	arguments=parser.parse_args();
	print(arguments);

def main():
	global arguments;
	parse_arguments();
	filename=arguments.filename;

	if arguments.starttime:
		date_format='%Y-%m-%d-%H:%M:%S';
		start=datetime.datetime.strptime(arguments.starttime, date_format);
	else:
		tomorrow=datetime.date.today() + datetime.timedelta(days=1);
		start=datetime.datetime(tomorrow.year,tomorrow.month,tomorrow.day,hour=7);

	print("read disc (track)");
	name,track=readtracks.readpoints(filename);
	print("make waypoints");
	
	
	print("read disc (waypoints)");
	Kgpx,Agpx=read_control_waypoints(filename);
	wpfinder=finder.Finder(track);
	K=project_waypoints(Kgpx,wpfinder);
	A=project_waypoints(Agpx,wpfinder);
	if not A:
		print("make automatic waypoints");
		A=automatic.waypoints(track);
	else:
		print(f"gpx has {len(A):d} automatic waypoints");
	W={**A, **K};
	W=filter_waypoints(W);
	W=label_waypoints(W,start,track);
	
	print("generate profile plot file");
	output.gnuplot_profile(track,W);
	print("generate map plot file");
	output.gnuplot_map(track,W);
	assert(W);
	output.latex_profile(W);
	if not os.path.exists("out"):
		os.makedirs("out");
	print("generate gpx");
	outfilename=arguments.output;
	if not arguments.output:
		outfilename="out/"+os.path.basename(filename);
	makegpx(track,W,name,outfilename);
	
main()

#!/usr/bin/env python3

import gpxpy;
import geopy.distance;
import datetime;
import math;
import sys;
import os;

sys.path.append(os.path.join(os.path.dirname(__file__),".."));

import elevation;
import readtracks;
import utils;
import finder;
import automatic;
from richwaypoint import RichWaypoint;

import output;

def read_control_waypoints(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	ret=list();
	for w in gpx.waypoints:
		rw=RichWaypoint(utils.Point(w.latitude,w.longitude,w.elevation));
		rw.type="K";
		rw.name=w.name;
		if w.description:
			rw.description=w.description.replace("’","'");
		if w.name:
			rw.name=w.name.strip();
		if not rw.name:
			rw.name="";
		ret.append(rw);	
	return ret;

def time_as_delta(hours):
	tddays=math.floor(hours/24);
	tdseconds=hours*3600 - tddays*24*3600;
	return datetime.timedelta(days=tddays,seconds=math.floor(tdseconds));

def timehours_to(distance):
	km=distance/1000;
	return km/15;


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

def control_waypoint_name(start,time,waypoint,counter):
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

def waypoint_string(w):
	#return f"{w.name:11s} {w.description:30s} lat:{w.latitude:2.5f} long:{w.longitude:2.5f}";
	return f"{w.name:11s};{w.description:30s}";



def project_waypoints(richpoints,finder):
	D=dict();
	for richpoint in richpoints:
		distance=finder.find_distance(richpoint.point);

		# we project all points, even controls,
		# because controls are exact points.
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


def dxdy(x,y,k1,k2):
	cumulative_x=0;
	cumulative_y=0;
	for k in range(k1,k2):
		# up
		if y[k]>y[k-1] and k>0:
			cumulative_y+=y[k]-y[k-1];
			assert(x[k]>x[k-1]);
			cumulative_x+=1000*(x[k]-x[k-1]);
	return cumulative_x,cumulative_y;
		

# slope
#	ret=dict();
#	K=summits(x,y);
#	assert(not 0 in K);
#	assert(len(K)>=1);
#	kprev=0;
#	for n in range(len(K)):
#		k=K[n];
#		dx,dy=dxdy(x,y,kprev,k);
#		assert(dy>=0);
#		assert(dx>0);
#		slope=100*dy/dx;

def label_waypoints(richpoints,start):
	D=dict();
	K=set();
	A=set();
	for distance in sorted(richpoints.keys()):
		richpoint=richpoints[distance];
		total_hours=timehours_to(distance);
		time=waypoint_time(total_hours,start);

		# save the original name in the description
		richpoint.description=richpoint.name;
		richpoint.name=control_waypoint_name(start,time,richpoint,len(D)+1);
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
		richpoint.time=time;
		if richpoint.isControlPoint():
			K.add(richpoint);
			richpoint.name=f"K{len(K):d}";
		else:
			A.add(richpoint);
			richpoint.name=f"A{len(A)%10:d}";
		D[distance]=richpoint;
	return D;


def makegpx(track,waypoints,name,filename):
	print("remove time and elevation before exporting to gpx");
	segment = toGPXSegment(track);
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

def main():
	if len(sys.argv)>1:
		filename=sys.argv[1];
	else:
		#filename=os.path.join(os.path.dirname(__file__),"..","..","test/elevation.gpx");
		filename=os.path.join(os.path.dirname(__file__),"..","..","test/blackforest.gpx");

	if len(sys.argv)>2:
		date_format='%Y-%m-%d-%H:%M:%S';
		start=datetime.datetime.strptime(sys.argv[2], date_format);
	else:
		tomorrow=datetime.date.today() + datetime.timedelta(days=1);
		start=datetime.datetime(tomorrow.year,tomorrow.month,tomorrow.day,hour=7);

	print("read disc (track)");
	name,track=readtracks.readpoints(filename);
	print("make waypoints");
	
	
	print("read disc (waypoints)");
	Kgpx=read_control_waypoints(filename);
	wpfinder=finder.Finder(track);
	K=project_waypoints(Kgpx,wpfinder);
	A=automatic.waypoints(track);
	#return;
	#last_point=RichWaypoint(track[-1]);
	#last_point.name="END";
	#gpxrichwaypoints.append(last_point);
	

	W={**A, **K};
	W=filter_waypoints(W);
	W=label_waypoints(W,start);
	
	print("generate profile plot file");
	output.gnuplot_profile(track,W);
	print("generate map plot file");
	output.gnuplot_map(track,W);
	assert(W);
	output.latex_profile(W);
	if not os.path.exists("out"):
		os.makedirs("out");
	print("generate gpx");
	makegpx(track,W,name,"out/"+os.path.basename(filename));
	
main()

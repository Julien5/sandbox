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
		else: # assume control point
			rw.type="K";
		if is_automatic(rw):
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
		# We project the waypoints on the track points.
		# Otherwise we cannot compute the distance.
		# TODO: fix that ?
		pr,index=finder.project(richpoint.point);
		d=utils.distance(pr,richpoint.point);
		richpoint.point=pr;
		richpoint.distance=distance;
		richpoint.index=index;
		if index in D:
			if richpoint.name == D[index].name:
				continue;
			new=richpoint;
			old=D[index];
			print("found waypoints with the same index:",index);
			print("new:",new.name,"-",new.description);
			print("old:",old.name,"-",old.description);
			assert(not index in D);
		D[index]=richpoint;
	return [D[d] for d in sorted(D.keys())];

def label_waypoints(richpoints,start,E):
	K=list();
	A=list();

	R=richpoints;

	D=dict();
	for k in range(len(R)):
		W=R[k];
		prefix=None;
		if W.isControlPoint():
			K.append(W);
			prefix=f"K{len(K):d}";
		else:
			A.append(W);
			prefix=f"A{len(A)%10:d}";
			
		Wprev=None;
		if k>0:
			Wprev=R[k-1]
			assert(W.distance>=Wprev.distance);
		
		total_hours=timehours_to(W.distance);
		time=waypoint_time(total_hours,start);
		time_str=fix_summer_winter_time(time).strftime("%H:%M");

		if not Wprev is None:
			dx,dy=E.slope(Wprev.index,W.index);
			W.dplus=dy;
			W.xdplus=dx;
			if dx>0:
				W.slope=100*dy/(W.distance-Wprev.distance);
		if not W.description:
			# save the original name in the description
			W.description=W.name;
		slope_str="  ";
		if not (W.slope is None):
			slope_str=f"{W.slope:2.0f}"
		W.name=f"{prefix:s}-{slope_str:s}-{time_str:s}";
		dp=-1;
		if not W.dplus is None:
			dp=W.dplus;
		#print(f"{W.name:s} at {W.distance/1000:3.0f} [{W.point.elevation:3.0f}m] D+={dp:3.0f} index:{W.index:d}");
		W.time=time;
		
		D[W.index]=W;
	return [D[i] for i in sorted(D.keys())];


def makegpx(track,waypoints,name,filename):
	global arguments;
	segment = toGPXSegment(track);
	if arguments.flat:
		print("remove time and elevation before exporting to gpx");
		for p in segment.walk(True):
			p.elevation=None;
			p.time=None;
		
	gpx = gpxpy.gpx.GPX();

	gpx_track = gpxpy.gpx.GPXTrack()
	gpx_track.name = name;
	gpx.tracks.append(gpx_track)
	gpx_track.segments.append(segment)
	
	L=[];
	for w in waypoints:
		# time=w.time;
		# print(f"{waypoint_string(w):s};{distance/1000:5.1f};{total_hours:3.1f}");
		L.append(toGPXWaypoint(w));
	gpx.waypoints=L;

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
	# Note: if two waypoints have the same distance, sorted[1] is not
	# necessarily the 'other closest'.
	S=sorted(W, key=lambda rw: abs(rw.index-w0.index));
	for k in range(2):
		if S[k] != w0:
			return S[k];
	assert(0);	
	return None;	
	
def short(w):
	return f"[{w.type:s}|{w.distance/1000:3.0f}km|{w.point.elevation:3.0f}m|{w.index:d}]"

def filter_waypoints(W):
	if not W:
		return W;
	for w in W:
		c=closest(W,w);
		d=abs(c.distance-w.distance);
		if d<4000:
			Sloc=sort_waypoints([w,c]);
			assert(len(Sloc)==2);
			winner=Sloc[0];
			looser=Sloc[-1];
			#print(f"examine {short(w):s} => hide {short(looser):s} because it is too close to {short(winner):s} (d={d:04.1f}m)");
			Sloc[-1].label_on_profile=False;
			if d<2000:
				Sloc[-1].hide=True;
	return [w for w in W if not w.hide];

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
	print("compute elevation");
	E=elevation.Elevation(track);
	
	print("read disc (waypoints)");
	Kgpx,Agpx=read_control_waypoints(filename);
	print("project points");
	wpfinder=finder.Finder(track);
	K=project_waypoints(Kgpx,wpfinder);
	A=project_waypoints(Agpx,wpfinder);
	if not A:
		print("make automatic waypoints");
		A=automatic.waypoints(track,E);
	else:
		print(f"gpx has {len(A):d} automatic waypoints");

	Wak=A+K;
	W=sorted(Wak, key=lambda w: w.distance);
	W=filter_waypoints(W);
	E.estimate_positive_elevation([w.index for w in W]);
	W=label_waypoints(W,start,E);
	
	print("generate profile plot file");
	output.gnuplot_profile(E,W);
	print("generate map plot file");
	assert(track);
	output.gnuplot_map(track,W);
	assert(W);
	
	outgpx=arguments.output;
	if not arguments.output:
		outgpx="out/"+os.path.basename(filename);
		if not os.path.exists("out"):
			os.makedirs("out");
	print("generate gpx",outgpx);
	makegpx(track,W,name,outgpx);
	outpdf=outgpx.replace(".gpx",".pdf");
	print("generate pdf",outpdf);
	output.latex_profile(E,W,outpdf);
	
	
main()

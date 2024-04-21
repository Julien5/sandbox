#!/usr/bin/env python3

import gpxpy;
import geopy.distance;
import datetime;
import math;
import sys;
import os;

import elevation;
import readtracks;

from scipy import spatial
import numpy as np


def readsegments(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	#assert(len(gpx.tracks)==1);
	R=list();
	for T in gpx.tracks:
		assert(len(T.segments)==1);
		name=T.name;
		#print(name)
		for segment in T.segments:
			for p in segment.points:
				p.extensions=None;
			R.append(segment);
	return R,name;

def join(S):
	R=gpxpy.gpx.GPXTrackSegment()
	for s in S:
		R.join(s);
	return R;	

def readwaypoints(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	for w in gpx.waypoints:
		if not w.description:
			w.description="";
		if not w.name:
			w.name="";
		w.name=w.name.strip();	
		w.description=w.description.replace("’","'");
		w.extensions=None;
	return gpx.waypoints;

def timedelta(hours):
	tddays=math.floor(hours/24);
	tdseconds=hours*3600 - tddays*24*3600;
	return datetime.timedelta(days=tddays,seconds=math.floor(tdseconds));

def timehours_to(distance):
	km=distance/1000;
	return km/15;

def timedelta_to(distance):
	return timedelta(timehours_to(distance));

def distlatlong(lat1,long1,lat2,long2):
	return geopy.distance.distance((lat1,long1),(lat2,long2)).m;

def dist(p1,p2):
	return distlatlong(p1.latitude,p1.longitude,p2.latitude,p2.longitude);

def waypoint_time(total_hours,start=None):
	delta=timedelta(total_hours);
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

def argmax(iterable):
    return max(enumerate(iterable), key=lambda x: x[1])[0]

def argmin(iterable):
    return min(enumerate(iterable), key=lambda x: x[1])[0]

class Point:
	def __init__(self,latitude,longitude):
		self.point = (latitude,longitude);

	def list(self):
		return [self.point[0], self.point[1]];

	def __hash__(self):
		return hash(self.point);

	def __eq__(self, other):
		return (self.point) == (other.point)

	def __ne__(self, other):
		return not(self == other)

	def dist(self,other):
		lat1=self.point[0];
		long1=self.point[1];
		lat2=other.point[0];
		long2=other.point[1];
		return distlatlong(lat1,long1,lat2,long2);
	
class Finder:
	def __init__(self,segment):
		G=segment.points;
		P=[Point(g.latitude,g.longitude) for g in G];
		self.dist=dict();
		self.dist[P[0]]=0;
		for k in range(len(P)-1):
			self.dist[P[k+1]] = self.dist[P[k]] + P[k].dist(P[k+1]);
		self.A = np.array([p.list() for p in P]);
		self.spatial = spatial.KDTree(self.A);

	def segment_length(self):
		return max(self.dist.values());

	def find_distance(self,p):
		pt=Point(p.latitude,p.longitude).list();
		index=self.spatial.query(pt)[1];
		nearest=self.A[index];
		[lat,longitude]=list(nearest);
		return self.dist[Point(lat,longitude)];

	def project(self,p):
		pt=Point(p.latitude,p.longitude).list();
		index=self.spatial.query(pt)[1];
		nearest=self.A[index];
		[lat,longitude]=list(nearest);
		return (lat,longitude);

def toWaypoint(latitude,longitude,elevation,name,description):
	w = gpxpy.gpx.GPXWaypoint()
	w.latitude=latitude;
	w.longitude=longitude;
	w.elevation=elevation;
	w.name=name;
	w.description=description;
	w.symbol = "Flag, Blue";
	return w;

def waypoint_string(w):
	#return f"{w.name:11s} {w.description:30s} lat:{w.latitude:2.5f} long:{w.longitude:2.5f}";
	return f"{w.name:11s};{w.description:30s}";

def process_waypoints(waypoints,finder,start):
	D=dict();
	for s in waypoints:
		point=s;
		distance=finder.find_distance(point);
		total_hours=timehours_to(distance);
		time=waypoint_time(total_hours,start);

		# we project all points, even controls,
		# because controls are exact points.
		(lat,longitude)=finder.project(point);
		point.latitude=lat;
		point.longitude=longitude;

		description=point.name;
		name=control_waypoint_name(start,time,s,len(D)+1);
		w=toWaypoint(point.latitude,point.longitude,point.elevation,name,description);
		if time in D:
			if w.name == D[time].name:
				continue;
			print("waypoint",w.name);
			print("another waypoing with the same time:",time,"name:",D[time].name)
			assert(not time in D);
		D[distance]=w;
	return D;

import pyproj
def gnuplot_map(P,W):
	myProj = pyproj.Proj("+proj=utm +zone=32K, +north +ellps=WGS84 +datum=WGS84 +units=m +no_defs")
	f=open("map-track.csv","w");
	for k in range(len(P)):
		lon=P[k].longitude;
		lat=P[k].latitude;
		x, y = myProj(lon, lat)
		f.write(f"{x:10.1f}\t{y:10.1f}\t{lat:10.6f}\t{lon:10.6f}\n");
	f.close();
	f=open("map-wpt.csv","w");
	for distance in W.keys():
		lon=W[distance].longitude;
		lat=W[distance].latitude;
		label=W[distance].name[:2];
		x, y = myProj(lon, lat)
		f.write(f"{x:10.1f}\t{y:10.1f}\t{lat:10.6f}\t{lon:10.6f}\t{label:s}\t{distance/1000:4.1f}\n");
	f.close();	


def gnuplot_profile(P,W):
	x,y=elevation.load(P);
	f=open("elevation.csv","w");
	for k in range(len(x)):
		f.write(f"{x[k]:5.2f}\t{y[k]:5.2f}\n");
	f.close();
	
	g=open("profile.gnuplot","r");
	labels_L=[];
	content=g.read();
	g.close();
	for line in content.split("\n"):
		if "#label " in line:
			labels_L.append(line);
	label_template="\n".join(labels_L);
	label_template=label_template.replace("#label ","");
	labels=list();
	for distance in W.keys():
		w=W[distance];
		label=label_template;
		wx=distance;
		wy=w.elevation;
		top=len(labels)%2 == 0;
		D=200;
		if top:
			label=label.replace("{labelyT1}",str(wy+D));
			label=label.replace("{labelyT2}",str(wy+(D-75)));
			label=label.replace("{labelya1}",str(wy+(D-100)));
			label=label.replace("{labelya2}",str(wy+5));
		else:
			label=label.replace("{labelyT1}",str(wy-D));
			label=label.replace("{labelyT2}",str(wy-(D-75)));
			label=label.replace("{labelya1}",str(wy-(D-100)));
			label=label.replace("{labelya2}",str(wy-5));
		label=label.replace("{labelname}",w.name[:2]);
		label=label.replace("{labelinfo}",f"{wy:3.0f}");
		label=label.replace("{labelx}",str(distance/1000));
		label=label.replace("{arrown}",str(len(labels)+1));
		labels.append(label);
	content=content.replace("#labels","\n".join(labels));	
	g=open("profile-out.gnuplot","w");
	g.write(content);
	g.close();


def automatic_waypoints(P,start):
	ret=dict();
	x,y=elevation.load(P);
	cumulative_y=0;
	segment_begin=0;
	segment_end=-1;
	counter=0;
	for k in range(len(x)):
		if k == 0:
			continue;
		
		dy=0;
		if y[k]>y[k-1]:
			dy=y[k]-y[k-1];
		cumulative_y+=dy;

		cumulative_x=1000*(x[k]-x[segment_begin]);

		if cumulative_y>=200: #  or cumulative_x>10000:
			segment_end=k;
			distance=1000*x[segment_end];
			slope=100*cumulative_y/cumulative_x;
			segment_begin=segment_end+1;
			# print(f"{cumulative_x/1000:3.1f} km | {slope:4.2f}%");

			counter+=1;
			if slope<10:
				slope_f=f"{slope*10:2.0f}";
			else:
				slope_f="..";

			total_hours=timehours_to(distance);
			time=waypoint_time(total_hours,start);
			assert(not time in ret.keys());
			time_str=fix_summer_winter_time(time).strftime("%H:%M");

			name=f"A{counter%10:d}-{slope_f:>2}-{time_str:s}";
			description="automatic"
			wp=toWaypoint(P[k].latitude,P[k].longitude,P[k].elevation,name,description);
			ret[distance]=wp;

			segment_end=-1;
			cumulative_y=0;
	return ret;

def makegpx(segment,waypoints,name,filename):
	gpx = gpxpy.gpx.GPX();

	gpx_track = gpxpy.gpx.GPXTrack()
	gpx_track.name = name;
	gpx.tracks.append(gpx_track)
	gpx_track.segments.append(segment)

	L=[];
	for distance in sorted(waypoints.keys()):
		w=waypoints[distance];
		total_hours=timehours_to(distance);
		print(f"{waypoint_string(w):s};{distance/1000:5.1f};{total_hours:3.1f}");
		L.append(w);
	gpx.waypoints=L;

	print("generate",filename);
	open(filename,'w').write(gpx.to_xml());

def main():
	if len(sys.argv)>1:
		filename=sys.argv[1];
	else:
		filename="test/elevation.gpx";

	if len(sys.argv)>2:
		date_format='%Y-%m-%d-%H:%M:%S';
		start=datetime.datetime.strptime(sys.argv[2], date_format);
	else:
		tomorrow=datetime.date.today() + datetime.timedelta(days=1);
		start=datetime.datetime(tomorrow.year,tomorrow.month,tomorrow.day,hour=7);

	P=readtracks.readpoints(filename);
	A=automatic_waypoints(P,start);
	gnuplot_profile(P,A);
	gnuplot_map(P,A);
	return;

	S,name=readsegments(filename);
	assert(len(S)==1);
	segment=S[0];
	
	for p in segment.walk(True):
		p.elevation=None;
		p.time=None;

	waypoints=readwaypoints(filename);
	last_point=segment.points[-1];
	waypoints.append(toWaypoint(last_point.latitude,last_point.longitude,last_point.elevation,"END",""));
	finder=Finder(segment);
	B=process_waypoints(waypoints,finder,start);
	W = {**A, **B};
	print(len(A),len(B),len(W));
	if not os.path.exists("out"):
		os.makedirs("out");
	makegpx(segment,W,name,"out/"+os.path.basename(filename));
	
main()

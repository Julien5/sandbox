#!/usr/bin/env python3

import gpxpy;
import geopy.distance;
import datetime;
import math;
import sys;
import os;
import subprocess;

import elevation;
import readtracks;
import utils;

from scipy import spatial
import numpy as np

def get_rwaypoints_at_page(k,W):
	xmin=max(100*k-10,0);
	xmax=100*(k+1)+10;
	R=dict();
	for distance in sorted(W.keys()):
		km=distance/1000;
		if xmin <= km and km <= xmax:
			R[distance]=W[distance];
	return R;	

class RichWaypoint:
	def __init__(self,waypoint,distance,time,t):
		self.waypoint=waypoint;
		self.distance=distance;
		self.time=time;
		self.type=t;

	def isControlPoint(self):
		return self.type == "K";

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
		D[distance]=RichWaypoint(w,distance,time,"K");
	return D;

def bbox(P,proj,d,dmin,dmax):
	assert(len(P)==len(d));
	kmin=None;
	kmax=None;
	for k in range(len(d)):
		if kmin is None or d[k]<dmin:
			kmin=k;
		if kmax is None or d[k]<dmax:
			kmax=k;
	U=[proj(p.longitude,p.latitude) for p in P[kmin:kmax]];
	margin=10000;
	xmin=min([u[0] for u in U])-margin;
	xmax=max([u[0] for u in U])+margin;
	ymin=min([u[1] for u in U])-margin;
	ymax=max([u[1] for u in U])+margin;
	return xmin,xmax,ymin,ymax;

def map_csv_waypoints(F,utm):
	L=[];
	for distance in F.keys():
		lon=F[distance].waypoint.longitude;
		lat=F[distance].waypoint.latitude;
		label=F[distance].waypoint.name[:2];
		x, y = utm(lon, lat)
		L.append(f"{x:10.1f}\t{y:10.1f}\t{lat:10.6f}\t{lon:10.6f}\t{label:s}\t{distance/1000:4.1f}");
	f=open("/tmp/profile/map-wpt.csv","w");
	f.write("\n".join(L));
	f.close();

def profile_csv_waypoints(F):
	L=list();
	assert(F);
	for distance in F.keys():
		w=F[distance];
		wx=w.distance/1000;
		wy=w.waypoint.elevation;
		label1=w.waypoint.name[:2];
		label2=f"{wy:4.0f}";
		L.append(f"{wx:5.2f}\t{wy:5.0f}\t{label1:s}\t{label2:s}");
	f=open("/tmp/profile/elevation-wpt.csv","w");
	f.write("\n".join(L));
	f.close();

import pyproj
def gnuplot_map(P,W):
	os.makedirs("/tmp/profile",exist_ok=True);
	utm = pyproj.Proj("+proj=utm +zone=32K, +north +ellps=WGS84 +datum=WGS84 +units=m +no_defs")
	d,dummy=elevation.load(P);
	
	L=[];
	for k in range(len(P)):
		lon=P[k].longitude;
		lat=P[k].latitude;
		x, y = utm(lon, lat)
		L.append(f"{x:10.1f}\t{y:10.1f}\t{lat:10.6f}\t{lon:10.6f}");
	f=open("/tmp/profile/map-track.csv","w");
	f.write("\n".join(L));
	f.close();

	f=open("gnuplot/map.gnuplot","r");
	content0=f.read();
	f.close();
	for k in range(math.floor(d[-1]/100)+1):
		Wk=get_rwaypoints_at_page(k,W);
		F=filter_waypoints(Wk);
		map_csv_waypoints(F,utm);
		xmin,xmax,ymin,ymax=bbox(P,utm,d,k*100,(k+1)*100);
		content=content0;
		content=content.replace("{pngx}",str(10*int((xmax-xmin)/1000)));
		content=content.replace("{pngy}",str(10*int((ymax-ymin)/1000)));
		content=content.replace("{xmin}",str(xmin));
		content=content.replace("{xmax}",str(xmax));
		content=content.replace("{ymin}",str(ymin));
		content=content.replace("{ymax}",str(ymax));
		filename=f"/tmp/profile/map-{k:d}.gnuplot";
		f=open(filename,"w");
		f.write(content);
		f.close();
		subprocess.run(["gnuplot",filename]);
		os.rename("/tmp/profile/map.png",f"/tmp/profile/map-{k:d}.png");
	return 	

import math;
def gnuplot_profile(P,W):
	os.makedirs("/tmp/profile",exist_ok=True);
	d,y=elevation.load(P);
	f=open("/tmp/profile/elevation.csv","w");
	for k in range(len(d)):
		f.write(f"{d[k]:5.2f}\t{y[k]:5.2f}\n");
	f.close();

	f=open("gnuplot/profile.gnuplot","r");
	content0=f.read();
	f.close();
	
	for xk in range(math.floor(d[-1]/100)+1):
		xmin=max(100*xk-10,0);
		xmax=100*(xk+1)+10;
		ymin=math.floor(min(y)/500)*500;
		ymax=math.ceil(max(y)/500)*500;
		content=content0;
		content=content.replace("{xmin}",str(xmin));
		content=content.replace("{xmax}",str(xmax));
		content=content.replace("{ymin}",str(ymin));
		content=content.replace("{ymax}",str(ymax));
		filename=f"/tmp/profile/profile-{xk:d}.gnuplot";
		f=open(filename,"w");
		f.write(content);
		f.close();
		Wk=get_rwaypoints_at_page(xk,W);
		assert(Wk);
		F=filter_waypoints(Wk);
		assert(F);
		profile_csv_waypoints(F);
		subprocess.run(["gnuplot",filename]);
		os.rename("/tmp/profile/profile.png",f"/tmp/profile/profile-{xk:d}.png");

def index_with(L,string):
	for k in range(len(L)):
		if string in L[k]:
			return k;
	assert(False);	
	return None;

def latex_waypoint(rwaypoint):
	L=list();
	L.append(rwaypoint.waypoint.name[:2]);
	L.append(f"{rwaypoint.distance/1000:3.1f} km");
	L.append(rwaypoint.time.strftime("%H:%M"));
	L.append(f"{rwaypoint.waypoint.elevation:3.1f}");
	separator=" & ";
	return separator.join(L);


def filter_waypoints(W):
	K=[W[d] for d in W.keys() if W[d].isControlPoint()];
	A=[W[d] for d in W.keys() if not W[d].isControlPoint()];
	for k in K:
		Aclose=[a for a in A if utils.distance(a.waypoint,k.waypoint)<5000];
		for a in Aclose:
			A.remove(a);
	namax=max(12-len(K),0);
	A2=[a for a in A if int(a.waypoint.name[1]) % 2 == 0];
	A5=[a for a in A if int(a.waypoint.name[1]) % 5 == 0];
	if len(A)<namax:
		pass;
	elif len(A2)<namax:
		pass;#A=A2;
	else:
		pass;#A=A5;
	R=dict();
	for w in A:
		R[w.distance]=w;
	for w in K:
		R[w.distance]=w;
	return R;

def latex_profile(W):
	f=open("tex/profile-template.tex","r");
	L=f.read().split("\n");
	f.close();
	start=index_with(L,"% begin-template");
	end=index_with(L,"% end-template");
	template0="\n".join(L[start:end+1]);
	filelist=os.listdir('/tmp/profile')
	k=0;
	out="\n".join(L);
	assert(template0 in out);
	parts=[];
	while True:
		template=template0;
		profilepng=f"/tmp/profile/profile-{k:d}.png";
		mappng=f"/tmp/profile/map-{k:d}.png";
		if not os.path.exists(profilepng):
			break;
		template=template.replace("{profile-png}",profilepng);
		template=template.replace("{map-png}",mappng);
		Wk=get_rwaypoints_at_page(k,W);
		pointlist=list();
		F=filter_waypoints(Wk);
		for d in sorted(F.keys()):
			w=F[d];
			pointlist.append(latex_waypoint(w));
		newline="\\\\";	
		template=template.replace("{pointlist}",f"{newline:s}\n\t".join(pointlist)+newline);
		parts.append(template);
		k=k+1;
	out=out.replace(template0,"\n".join(parts));
	f=open("/tmp/profile/profile.tex","w");
	f.write(out);
	f.close();

def argmax(x,R):
	return max(R, key=lambda i: x[i])

def argmin(x,R):
	return min(R, key=lambda i: x[i])

def R20(x,start):
	return [k for k in range(start,len(x)) if abs(x[start]-x[k])<10];

def summits(x,y):
	start=0;
	ret=list();
	while start<len(x):
		R=R20(x,start);
		kmax=argmax(y,R);
		start=max(R)+1;
		ret.append(kmax);
	return ret;

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
		
def automatic_waypoints(P,start):
	ret=dict();
	x,y=elevation.load(P);
	K=summits(x,y);
	assert(not 0 in K);
	assert(len(K)>=1);
	kprev=0;
	counter=0;
	for n in range(len(K)):
		k=K[n];
		dx,dy=dxdy(x,y,kprev,k);
		assert(dy>=0);
		assert(dx>0);
		slope=100*dy/dx;
		if slope<10:
			slope_f=f"{slope*10:2.0f}";
		else:
			slope_f="..";
		distance=x[k]*1000;
		total_hours=timehours_to(distance);
		time=waypoint_time(total_hours,start);
		assert(not time in ret.keys());
		time_str=fix_summer_winter_time(time).strftime("%H:%M");
		name=f"A{counter%10:d}-{slope_f:>2}-{time_str:s}";
		description="automatic"
		wp=toWaypoint(P[k].latitude,P[k].longitude,P[k].elevation,name,description);
		ret[distance]=RichWaypoint(wp,distance,time,"A");
		kprev=k;
		counter+=1;
	return ret;

def makegpx(segment,waypoints,name,filename):
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
	for distance in sorted(waypoints.keys()):
		time=waypoints[distance].time;
		# print(f"{waypoint_string(w):s};{distance/1000:5.1f};{total_hours:3.1f}");
		L.append(waypoints[distance].waypoint);
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
	print("read disc");
	P=readtracks.readpoints(filename);
	print("make waypoints");
	A=automatic_waypoints(P,start);
	print("read disc again (track)");
	S,name=readsegments(filename);
	assert(len(S)==1);
	segment=S[0];
	print("read disc again (wpt)");
	waypoints=readwaypoints(filename);
	last_point=segment.points[-1];
	print("process waypoints");
	waypoints.append(toWaypoint(last_point.latitude,last_point.longitude,last_point.elevation,"END",""));
	finder=Finder(segment);
	B=process_waypoints(waypoints,finder,start);
	W = {**A, **B};
	print("generate profile plot file");
	gnuplot_profile(P,W);
	print("generate map plot file");
	gnuplot_map(P,W);
	latex_profile(W);
	if not os.path.exists("out"):
		os.makedirs("out");
	print("generate gpx");
	makegpx(segment,W,name,"out/"+os.path.basename(filename));
	
main()

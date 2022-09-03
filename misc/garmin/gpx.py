#!/usr/bin/env python3

import gpxpy
import gpxpy.gpx
import sys;
import os;
import xml.etree.cElementTree as mod_etree

def save_segment(seg,name):
	gpx = gpxpy.gpx.GPX()

	gpx_track = gpxpy.gpx.GPXTrack()
	gpx.tracks.append(gpx_track)

	gpx_segment = gpxpy.gpx.GPXTrackSegment()
	gpx_track.segments.append(seg);
	gpx_track.name=name;
	filename=name;
	if not ".gpx" in filename:
		filename="o-"+name+".gpx";
	f=open(filename,'w');
	f.write(gpx.to_xml());
	f.close();

def save_waypoints(W,filename):
	gpx = gpxpy.gpx.GPX()
	for w in W:
		gpx.waypoints.append(w);
	f=open(filename,'w');
	f.write(gpx.to_xml());
	f.close();

def to_routepoint(w):
	p=gpxpy.gpx.GPXRoutePoint();
	p.latitude=w.latitude;
	p.longitude=w.longitude;
	p.name = "{:02d}".format(int(w.name));
	p.symbol = w.symbol;
	p.extensions=w.extensions;
	# p = w;
	return p;

def waypoints_to_route(W,name="route"):
	gpx = gpxpy.gpx.GPX()
	R = gpxpy.gpx.GPXRoute();
	R.name=name;
	for w in W:
		R.points.append(to_routepoint(w))
	gpx.routes.append(R);
	return R;

def save_route(R,filename):
	gpx = gpxpy.gpx.GPX()
	gpx.routes.append(R);
	f=open(filename,'w');
	f.write(gpx.to_xml());
	f.close();

def is_poi(p):
	s=p.name;
	if not s:
		return False;
	# filter out coordinate-named points.
	if "," in s and "." in s:
		return False;
	s=s.replace(",","");
	s=s.replace(".","");
	return not s.isnumeric();

def trackpoint_to_waypoint(s):
	w = gpxpy.gpx.GPXWaypoint();
	w.name = s.name;
	w.symbol = "City (Small)";
	w.latitude=s.latitude;
	w.longitude=s.longitude;
	return w;

def pois_from_segment(segment):
	N=len(segment.points);
	W=[];
	for i in range(N):
		p=segment.points[i];
		if not is_poi(p):
			continue;
	
		W.append(p);
	return W;	
			
def clean(segment):
	N=len(segment.points);
	p_prev=None
	ascent=0;
	descent=0;
	for i in range(N):
		p=segment.points[i];
		for e in p.extensions:
			print(type(e))
			print("tag:",e.tag)
			print("a:",e.attrib)
			print("t:",e.text);
			if "outdooractive" in e.tag:
				e.tag="OA";
		if p_prev:		
			p1=p_prev;
			p2=p;
			d=p2.elevation-p1.elevation;
			if d>0:
				ascent+=d;
			else:
				descent-=d;
		p_prev=p;		
	print("ascent:",ascent);
	print("descent:",descent);	
		
def restart(segment):
	T = gpxpy.gpx.GPXTrackSegment()
	start_index=None;
	N=len(segment.points);
	for i in range(N):
		point=segment.points[i];
		if point.name=="start":
			print('found start at ({0},{1})'.format(point.latitude, point.longitude));
			start_index=i;
	if not start_index:
		return segment;
	indexes=[i+start_index for i in range(N)];		
	for i in indexes:
		if i<N:
			p=segment.points[i];
		else:
			p=segment.points[i-N];
		p.extensions=None; # buggy in xml export.
		T.points.append(p);
	return T;

def simplify(seg,maxcount=None):
	infile="/tmp/simplify.input.gpx";
	outfile="/tmp/simplify.output.gpx";
	for f in [infile,outfile]:
		if os.path.exists(infile):
			os.remove(infile);
	save_segment(seg,infile);
	N=len(seg.points);
	if not maxcount:
		maxcount=min(int(N*0.09),200);
	count=maxcount;	
	cmd="gpsbabel -i gpx -f {} -x simplify,count={} -o gpx -F {}".format(infile,count,outfile);
	os.system(cmd);
	gpx_file = open(outfile, 'r')
	gpx = gpxpy.parse(gpx_file)
	assert(len(gpx.tracks)==1);
	track=gpx.tracks[0];
	assert(len(track.segments)==1);
	print("simplified to",len(track.segments[0].points),"points");	
	return track.segments[0];

def add_alarm_extension(w,meters):
	e = mod_etree.Element(f'wptx1:WaypointExtension');
	proximity = mod_etree.SubElement(e,f'wptx1:Proximity');
	proximity.text = str(float(meters))
	w.extensions.append(e);

def segment_to_waypoints(S,with_alarm=False):
	W = [];
	for i in range(len(S.points)):
		s = S.points[i];
		w = gpxpy.gpx.GPXWaypoint();
		w.name = "{:03d}".format(i);
		w.symbol = "City (Small)";
		w.latitude=s.latitude;
		w.longitude=s.longitude;
		if with_alarm:
			add_alarm_extension(w,200);	
		W.append(w);
	return W;

def main(filename):
	gpx_file = open(filename, 'r')
	gpx = gpxpy.parse(gpx_file)
	assert(len(gpx.tracks)==1);
	track=gpx.tracks[0];
	print(len(track.segments))
	#assert(len(track.segments)==1);
	segment=track.segments[0];
	print("track has",len(segment.points),"points.");
	clean(segment);
	outgpx = gpxpy.gpx.GPX();
	for poi in pois_from_segment(segment):
		outgpx.waypoints.append(poi);
	T = restart(segment);
	name=track.name;
	save_segment(T,name);

	S = simplify(T,100);
	S50 = simplify(T,15);

	W = segment_to_waypoints(S50);
	Wa = segment_to_waypoints(S50,True);
	save_waypoints(W,"o-waypoints-"+name+".gpx");

	R = waypoints_to_route(segment_to_waypoints(S50),name);
	save_route(R,"o-route-"+name+".gpx");
	
if __name__ == '__main__':
	print(sys.argv);
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

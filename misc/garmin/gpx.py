#!/usr/bin/env python3

import gpxpy
import gpxpy.gpx
import sys;
import os;
import xml.etree.cElementTree as mod_etree

def save_segment(seg,filename):
	gpx = gpxpy.gpx.GPX()

	gpx_track = gpxpy.gpx.GPXTrack()
	gpx.tracks.append(gpx_track)

	gpx_segment = gpxpy.gpx.GPXTrackSegment()
	gpx_track.segments.append(seg);
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

def restart(segment):
	T = gpxpy.gpx.GPXTrackSegment()
	start_index=None;
	N=len(segment.points);
	for i in range(N):
		point=segment.points[i];
		if point.name=="start":
			print('found start at ({0},{1})'.format(point.latitude, point.longitude));
			start_index=i;
	assert(start_index);
	indexes=[i+start_index for i in range(N)];		
	for i in indexes:
		if i<N:
			p=segment.points[i];
		else:
			p=segment.points[i-N];
		p.extensions=None; # buggy in xml export.
		T.points.append(p);
	return T;

def simplify(seg):
	infile="/tmp/simplify.input.gpx";
	outfile="/tmp/simplify.output.gpx";
	for f in [infile,outfile]:
		if os.path.exists(infile):
			os.remove(infile);
	save_segment(seg,infile);
	count=min(len(seg.points)/10,200);
	cmd="gpsbabel -i gpx -f {} -x simplify,count={} -o gpx -F {}".format(infile,count,outfile);
	print(cmd)
	os.system(cmd);
	gpx_file = open(outfile, 'r')
	gpx = gpxpy.parse(gpx_file)
	assert(len(gpx.tracks)==1);
	track=gpx.tracks[0];
	assert(len(track.segments)==1);
	print("new length:",len(track.segments[0].points));
	return track.segments[0];

def segment_to_waypoints(S,with_alarm=False):
	W = [];
	for i in range(len(S.points)):
		s = S.points[i];
		w = gpxpy.gpx.GPXWaypoint();
		w.name = "{:03d}".format(i);
		if with_alarm:
			w.symbol = "City (Small)";
		w.latitude=s.latitude;
		w.longitude=s.longitude;
		if with_alarm:
			e = mod_etree.Element(f'wptx1:WaypointExtension');
			proximity = mod_etree.SubElement(e,f'wptx1:Proximity');
			proximity.text = "200.0";
			w.extensions.append(e);
		W.append(w);
	return W;

def main(filename):
	gpx_file = open(filename, 'r')
	gpx = gpxpy.parse(gpx_file)
	assert(len(gpx.tracks)==1);
	track=gpx.tracks[0];
	assert(len(track.segments)==1);
	segment=track.segments[0];
	T = restart(segment);
	save_segment(T,"restarted.gpx");

	S = simplify(T);
	W = segment_to_waypoints(S);
	Wa = segment_to_waypoints(S,True);
	save_waypoints(W,"waypoints.gpx");
	save_waypoints(Wa,"waypoints-alarm.gpx");	
	
if __name__ == '__main__':
	print(sys.argv);
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

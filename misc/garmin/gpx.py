#!/usr/bin/env python3

import gpxpy
import gpxpy.gpx
import sys;
import os;

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
	W = [];
	for s in S.points:
		w = gpxpy.gpx.GPXWaypoint();
		w.latitude=s.latitude;
		w.longitude=s.longitude;
		W.append(w);
		
	save_waypoints(W,"waypoints.gpx");	
	
if __name__ == '__main__':
	print(sys.argv);
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

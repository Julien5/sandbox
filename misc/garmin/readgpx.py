#!/usr/bin/env python3

import track;

import gpxpy
import gpxpy.gpx
import sys;
import os;
import xml.etree.cElementTree as mod_etree

def clean(segment):
	N=len(segment.points);
	p_prev=None
	for i in range(N):
		p=segment.points[i];
		for e in p.extensions:
			# this is what cause problems with gpsbabel
			if "outdooractive" in e.tag:
				e.tag="OA";
				
def extract_segment(gpx):
	if len(gpx.tracks) != 1:
		print("could not find unique track.");	
		sys.exit(1);
	track=gpx.tracks[0];
	if len(track.segments) != 1:
		print("could not find unique segment.");	
		sys.exit(1);
	assert(len(track.segments)==1);
	segment=track.segments[0];
	return track.name,segment;

def readgpx(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	return gpx;

def to_track(segment,filename):
	ret=track.Track(filename);
	for point in segment.points:
		latitude=point.latitude;
		longitude=point.longitude;
		# point.elevation
		time = point.time;
		ret.append(time,track.Point(latitude,longitude));
	return ret;	
		
def tracks(filename):
	gpx = readgpx(filename);
	ret = list();
	for T in gpx.tracks:
		for S in T.segments:
			clean(S);
			ret.append(to_track(S,filename));
	return ret;	 

def main(filename):
	tracks(filename);
	
if __name__ == '__main__':
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

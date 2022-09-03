#!/usr/bin/env python3

import gpxpy
import gpxpy.gpx
import sys;
import os;
import xml.etree.cElementTree as mod_etree

def save_segment(segment,filename):
	gpx = gpxpy.gpx.GPX();

	# save track
	gpx_track = gpxpy.gpx.GPXTrack()
	gpx.tracks.append(gpx_track)
	gpx_segment = gpxpy.gpx.GPXTrackSegment()
	gpx_track.segments.append(segment);

	f=open(filename,'w');
	f.write(gpx.to_xml());
	f.close();	

def is_good_waypoint(p):
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
		if not is_good_waypoint(p):
			continue;
		W.append(trackpoint_to_waypoint(p));
	return W;	
			
def clean(segment):
	N=len(segment.points);
	p_prev=None
	ascent=0;
	descent=0;
	for i in range(N):
		p=segment.points[i];
		for e in p.extensions:
			# this is what cause problems with gpsbabel
			if "outdooractive" in e.tag:
				e.tag="OA";
				
def simplify(seg,maxcount):
	infile="/tmp/simplify.input.gpx";
	outfile="/tmp/simplify.output.gpx";
	for f in [infile,outfile]:
		if os.path.exists(infile):
			os.remove(infile);
	save_segment(seg,infile);
	count=maxcount;	
	cmd="gpsbabel -i gpx -f {} -x simplify,count={} -o gpx -F {}".format(infile,count,outfile);
	os.system(cmd);
	gpx_file = open(outfile, 'r')
	gpx = gpxpy.parse(gpx_file)
	assert(len(gpx.tracks)==1);
	track=gpx.tracks[0];
	assert(len(track.segments)==1);
	# print("simplified to",len(track.segments[0].points),"points");	
	return track.segments[0];

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

class GPX:
	def __init__(self,filename):
		self.filename=filename;
		self.igpx = readgpx(filename);
		
	def process(self):
		self.name,segment=extract_segment(self.igpx);
		clean(segment);
		self.waypoints=pois_from_segment(segment);
		self.track = simplify(segment,8192);

	def save(self):
		filename = self.name+".gpx";
		gpx = gpxpy.gpx.GPX();

		# save track
		gpx_track = gpxpy.gpx.GPXTrack()
		gpx.tracks.append(gpx_track)
		gpx_segment = gpxpy.gpx.GPXTrackSegment()
		gpx_track.segments.append(self.track);
		gpx_track.name=self.name;

		# save waypoints
		for w in self.waypoints:
			gpx.waypoints.append(w);

		print("write",filename);	
		f=open(filename,'w');
		f.write(gpx.to_xml());
		f.close();

def main(filename):
	g = GPX(filename);
	g.process();
	g.save();
	
if __name__ == '__main__':
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

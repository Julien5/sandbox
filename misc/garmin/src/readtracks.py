#!/usr/bin/env python3

import gpxpy
import gpxpy.gpx

import os;
import datetime;
import statistics;

import utils;

def readgpx(filename):
	gpx_file = open(filename, 'r');
	gpx = gpxpy.parse(gpx_file);
	gpx_file.close();
	return gpx;

def writegpx(filename,gpx):
	f=open(filename,'w');	
	f.write(gpx.to_xml());
	f.close();

def PointFromGPXPY(p):
	return utils.Point(p.latitude,p.longitude,p.elevation,p.time);

def writepoints(points,filename):
	gpx = gpxpy.gpx.GPX();
	gpx_track = gpxpy.gpx.GPXTrack()
	gpx.tracks.append(gpx_track)
	gpx_segment = gpxpy.gpx.GPXTrackSegment()
	gpx_track.segments.append(gpx_segment)
	# Create points:
	for p in points:
		lat=p.latitude;	
		lon=p.longitude;
		elev=p.elevation;
		time=p.time;
		gpx_segment.points.append(gpxpy.gpx.GPXTrackPoint(lat,lon,elev,time));
	writegpx(filename,gpx);

def segpoints(segment):
	ret=list();
	for point in segment.points:
		# assert(point.time);
		p=PointFromGPXPY(point);
		ret.append(p);
	return ret;

def readpoints(path):
	gpx = readgpx(path);
	ret=list();
	name=None;
	for track in gpx.tracks:
		name=track.name;
		for seg in track.segments:
			ret.extend(segpoints(seg));
	return name,ret,gpx.creator;		



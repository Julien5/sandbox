#!/usr/bin/env python3

import track;
import gpxpy.gpx

def writegpx(filename,gpx):
	f=open(filename,'w');	
	f.write(gpx.to_xml());
	f.close();

def write(track,filename):
	gpx = gpxpy.gpx.GPX();
	gpx_track = gpxpy.gpx.GPXTrack()
	gpx.tracks.append(gpx_track)
	gpx_segment = gpxpy.gpx.GPXTrackSegment()
	gpx_track.segments.append(gpx_segment)
	# Create points:
	P=track.points();
	for p in P:
		lat=p.latitude();	
		lon=p.longitude();
		elev=p.elevation();
		time=p.time();
		gpx_segment.points.append(gpxpy.gpx.GPXTrackPoint(lat,lon,elev,time));
	writegpx(filename,gpx);	

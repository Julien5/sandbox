#!/usr/bin/env python3

import gpxpy
import gpxpy.gpx
import sys;

def save_segment(seg,filename):
	gpx = gpxpy.gpx.GPX()

	gpx_track = gpxpy.gpx.GPXTrack()
	gpx.tracks.append(gpx_track)

	gpx_segment = gpxpy.gpx.GPXTrackSegment()
	gpx_track.segments.append(seg);
	f=open(filename,'w');
	f.write(gpx.to_xml());
	f.close();

def main(filename):
	gpx_file = open(filename, 'r')
	gpx = gpxpy.parse(gpx_file)

	assert(len(gpx.tracks)==1);
	track=gpx.tracks[0];
	assert(len(track.segments)==1);
	segment=track.segments[0];
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
		p.extensions=None;
		T.points.append(p);	
	save_segment(T,"restarted.gpx");
			
	for waypoint in gpx.waypoints:
		print('waypoint {0} -> ({1},{2})'.format(waypoint.name, waypoint.latitude, waypoint.longitude))

	for route in gpx.routes:
		print('Route:')
		for point in route.points:
			print('Point at ({0},{1}) -> {2}'.format(point.latitude, point.longitude, point.elevation))		
		
if __name__ == '__main__':
	print(sys.argv);
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

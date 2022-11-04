#!/usr/bin/env python3

import pyproj
import math

#P = pyproj.Proj(proj='utm', zone=32, ellps='WGS84', preserve_units=True)
#P = pyproj.Proj("+proj=utm +zone=32, +north +ellps=WGS84 +datum=WGS84 +units=m +no_defs")
G = pyproj.Geod(ellps='WGS84')

toUTM=pyproj.Transformer.from_crs("EPSG:4326", "EPSG:32632")
fromUTM=pyproj.Transformer.from_crs("EPSG:32632","EPSG:4326")

def convert(Lat,Lon):
	return toUTM.transform(Lat,Lon);
    # return P(Lat,Lon)    

def XY_To_LatLon(x,y):
	return fromUTM.transform(x,y);
    # return P(x,y,inverse=True)    

def distance(Lat1, Lon1, Lat2, Lon2):
    return G.inv(Lon1, Lat1, Lon2, Lat2)[2]

def test():
	# EPSG:32632
	# 32 N 552217     5316537
	# 32 U 552216.718 5316537.274
	lat=48.0;
	long=9.7;
	(x,y)=convert(lat,long);
	print(x,y);
	print(XY_To_LatLon(x,y));

if __name__ == '__main__':
	test();

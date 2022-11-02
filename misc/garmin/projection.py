#!/usr/bin/env python3


import pyproj
import math

P = pyproj.Proj(proj='utm', zone=32, ellps='WGS84', preserve_units=True)
G = pyproj.Geod(ellps='WGS84')

def convert(Lat,Lon):
    return P(Lat,Lon)    

def XY_To_LatLon(x,y):
    return P(x,y,inverse=True)    

def distance(Lat1, Lon1, Lat2, Lon2):
    return G.inv(Lon1, Lat1, Lon2, Lat2)[2]

def test():
	lat=48.0;
	long=9.7;
	(x,y)=convert(lat,long);
	print(x,y);
	print(XY_To_LatLon(x,y));

if __name__ == '__main__':
	test();

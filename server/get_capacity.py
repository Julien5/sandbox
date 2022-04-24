#!/usr/bin/env python3

import data;
import sys;
import datetime;
sys.path.append("deserialization")
import capacity;

from readints import *;

def get_capacity(sql,day):
	R=sql.read_request("/compteur/capacity",day);
	C=[];
	for r in R:
		bytes=r.data;
		time=r.time;
		c=capacity.Capacity(bytes,time);
		C.append(c);
	return C;

def exportcsv(sql):
	startday=datetime.datetime.now()-datetime.timedelta(days=10);
	C=get_capacity(sql,startday);
	line_time=["none"]
	line_esp=["esp"]
	line_ard=["arduino"]
	for c in C:
		line_time.append(c.time.strftime("%H:%M"));
		line_esp.append(str(c.esp));
		line_ard.append(str(c.arduino));
	csv=",".join(line_time);
	csv+="\n";
	csv+=",".join(line_ard);
	#csv+="\n";
	#csv+=",".join(line_esp);
	return csv;

def main():
	sql=data.Sql();
	startday=datetime.datetime.now()-datetime.timedelta(days=10);
	C=get_capacity(sql,startday);
	for c in C:
		print(c)
	print(exportcsv(sql))	
					
if __name__ == "__main__":
	main();


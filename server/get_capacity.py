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

def attime(C,time):
	for k in range(len(C)-1):
		if C[k].time<=time and time<=C[k+1].time:
			return C[k+1];
	return None;

def exportcsv(sql):
	end=datetime.datetime.now();	
	beg=end-datetime.timedelta(days=10);
	C=get_capacity(sql,beg);
	line_time=["none"]
	line_esp=["esp"]
	line_ard=["arduino"]
	time=beg;
	step=datetime.timedelta(seconds=6*3600);
	while True:
		print(len(line_time))	
		print("time",time)	
		c=attime(C,time);
		time+=step;
		if time>end:
			break;
		if c is None:
			continue;
		line_time.append(time.strftime("%d [%H]"));
		line_esp.append(str(c.esp));
		line_ard.append(str(c.arduino));

	
	csv=",".join(line_time);
	csv+="\n";
	csv+=",".join(line_ard);
	csv+="\n";
	csv+=",".join(line_esp);
	return csv;

def main():
	sql=data.Sql();
	startday=datetime.datetime.now()-datetime.timedelta(days=10);
	C=get_capacity(sql,startday);
	for c in C:
		print(c)
	print("export")	
	print(exportcsv(sql))	
					
if __name__ == "__main__":
	main();


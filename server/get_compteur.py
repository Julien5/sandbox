#!/usr/bin/env python3

import data;
import sys;
import datetime;
sys.path.append("deserialization")
import tickscounter;

from readints import *;

def get_tickscounter(sql):
	R=sql.read_request("/compteur/tickcounter",datetime.datetime.now());
	Q=[];
	for r in R:
		bytes=r.data;
		pos=0;
		index=0;
		epoch_at_start=read_ui64(bytes,pos); pos+=8;
		millis_at_end=read_ui64(bytes,pos); pos+=8;
		bins=tickscounter.readbins(bytes[pos:]);
		q=tickscounter.tickscounter(epoch_at_start,millis_at_end,bins,r.time);
		Q.append(q);
	return Q;	

def power(Q,start,end):
	R=[];	
	for q in Q:
		R.extend(q.midbins(start,end));
	d=end-start;
	dsec=d.total_seconds();
	count=sum([r.count for r in R]);
	perhour=3600*count/dsec;
	W=1000*perhour/70.0;
	return W;

def get_tickscounter_text(sql):
	Q=get_tickscounter(sql);	
	return "\n".join([str(q) for q in Q]);	
	
def main():
	sql=data.Sql();
	print(get_tickscounter_text(sql));

def exportcsv(t1,t2):
	sql=data.Sql();
	Q=get_tickscounter(sql);
	start=t1;
	step=datetime.timedelta(seconds=60*5);
	while start < t2:
		print(start,power(Q,start,start+step));
		start+=step;
			
if __name__ == "__main__":
	main();
	start=datetime.datetime.fromisoformat("2022-04-22 20:00:00");
	end=datetime.datetime.fromisoformat("2022-04-22 22:00:00");
	exportcsv(start,end);

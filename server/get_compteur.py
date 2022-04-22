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
		T=tickscounter.tickscounter(epoch_at_start,millis_at_end,bins,r.time);
		for b in T.getbins():
			Q.append(b);
	return Q;	

def closest_before(Q,t):
	ret=None;
	dmin=datetime.timedelta(days=100);
	for q in Q:
		if not q.count:
			continue;
		d=t-q.end();
		if d.total_seconds()<=0:
			continue;	
		if d<dmin:
			ret=q;
			dmin=d;
	return ret;

def closest_after(Q,t):
	ret=None;
	dmin=datetime.timedelta(days=100);
	for q in Q:
		if not q.count:
			continue;	
		d=q.start-t;
		if d.total_seconds()<0:
			continue;	
		if d<dmin:
			ret=q;
			dmin=d;
	return ret;		

def power(Q,start,end):
	R=[];	
	for q in Q:
		m=q.midbin(start,end);
		if m:
			R.append(m);
	zero=datetime.timedelta();
	if not R:
		A=closest_before(Q,start);
		B=closest_after(Q,end);
		if A and B:
			R=[tickscounter.bin(A.end(),1,zero),tickscounter.bin(B.start,1,zero)];
		else:
			return 0;	
	count=sum([r.count for r in R]);
	if count == 1:
		assert(len(R)==1);
		A=closest_before(Q,R[0].start);
		R=[tickscounter.bin(A.end(),1,zero),R[0]];
		
	d=R[-1].end()-R[0].start;
	dsec=d.total_seconds();
	count=sum([r.count for r in R]);
	if not dsec:
		print("bug")	
		for r in R:	
			print(r);
	perhour=3600*(count-1)/dsec;
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
	step=datetime.timedelta(seconds=60);
	while start < t2:
		p=power(Q,start,start+step);
		print(start,p);
		start+=step;
			
if __name__ == "__main__":
	main();
	start=datetime.datetime.fromisoformat("2022-04-22 20:10:00");
	end=datetime.datetime.fromisoformat("2022-04-22 22:00:00");
	exportcsv(start,end);

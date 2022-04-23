#!/usr/bin/env python3

import data;
import sys;
import datetime;
sys.path.append("deserialization")
import tickscounter;

from readints import *;

def get_tickscounter(sql,day):
	R=sql.read_request("/compteur/tickcounter",day);
	Q=[];
	for r in R:
		bytes=r.data;
		pos=0;
		index=0;
		epoch_at_start=read_ui64(bytes,pos); pos+=8;
		millis_at_end=read_ui64(bytes,pos); pos+=8;
		bins=tickscounter.readbins(bytes[pos:]);
		T=tickscounter.tickscounter(epoch_at_start,millis_at_end,bins,r.time);
		Q.append(T);
	return Q;

def get_bins(sql,day):
	Q=[];	
	T=get_tickscounter(sql,day);	
	for t in T:
		if t.count() == 0:
			continue;
		for b in t.getbins():
			Q.append(b);
	return Q;

def get_tickscounter_text(sql):
	day=datetime.datetime.now();
	Q=get_tickscounter(sql,day);
	return "\n".join([str(q) for q in Q if q.count()>0]);	

def ticks(bins):
	T=list();
	for b in bins:
		T.extend(b.ticks());
	return T;

def find_interval(T,t):
	for k in range(len(T)-1):
		if T[k] <= t and t <= T[k+1]:
			return [T[k],T[k+1]];
	return None;

def power(T,t):
	assert(T);	
	I=find_interval(T,t);
	if not I:
		return None;	
	[t1,t2]=I;
	d=t2-t1;
	dsec=d.total_seconds();
	perhour=3600/dsec;
	W=1000*perhour/70.0;
	return W;

def main():
	sql=data.Sql();
	day=datetime.datetime.fromisoformat("2022-04-22");
	Q=get_bins(sql,day);
	print(len(Q))
	#print(ticks(Q))

def exportcsv(t1,t2):
	sql=data.Sql();
	Q=get_bins(sql,t2);
	t=t1;
	step=datetime.timedelta(seconds=60);
	line_time=["none"]
	line_power=["power"]
	while t < t2:
		p=power(ticks(Q),t);
		t+=step;
		if p is None:
			continue;
		line_time.append(t.strftime("%H:%M:%S"));
		line_power.append(str(p));
		
	csv=",".join(line_time);
	csv+="\n";
	csv+=",".join(line_power);
	return csv;
					
if __name__ == "__main__":
	main();
	start=datetime.datetime.fromisoformat("2022-04-23 06:00:00");
	end=datetime.datetime.fromisoformat("2022-04-23 09:00:00");
	end=datetime.datetime.now();
	start=end-datetime.timedelta(hours=3);
	csv=exportcsv(start,end);
	open("csv/data2.csv","w").write(csv);

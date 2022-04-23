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
		for b in T.getbins():
			Q.append(b);
	return Q;

def get_tickscounter_text(sql):
	Q=get_tickscounter(sql);	
	return "\n".join([str(q) for q in Q]);	

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
		return 0;	
	[t1,t2]=I;
	d=t2-t1;
	dsec=d.total_seconds();
	perhour=3600/dsec;
	W=1000*perhour/70.0;
	return W;

def main():
	sql=data.Sql();
	day=datetime.datetime.fromisoformat("2022-04-22");
	Q=get_tickscounter(sql,day);
	print(len(Q))
	print(ticks(Q))

def exportcsv(t1,t2):
	sql=data.Sql();
	Q=get_tickscounter(sql,t2);
	t=t1;
	step=datetime.timedelta(seconds=10*60);
	line_time=["none"]
	line_power=["power"]
	while t < t2:
		p=power(ticks(Q),t);
		line_time.append(t.strftime("%H:%M"));
		line_power.append(str(p));
		t+=step;
	csv=",".join(line_time);
	csv+="\n";
	csv+=",".join(line_power);
	return csv;
					
if __name__ == "__main__":
	main();
	start=datetime.datetime.fromisoformat("2022-04-22 01:40:00");
	end=datetime.datetime.fromisoformat("2022-04-22 23:00:00");
	csv=exportcsv(start,end);
	open("csv/data2.csv","w").write(csv);

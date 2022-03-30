#!/usr/bin/env python3

import data;
import sys;
sys.path.append("deserialization")
import tickscounter;

from readints import *;

def get_tickscounter(sql,n):
	R=sql.read_request("/compteur/tickcounter/data");
	Q=[];
	for r in R[-n:]:
		bytes=r.data;
		pos=0;
		index=0;
		epoch_at_start=read_ui64(bytes,pos); pos+=8;
		millis_at_end=read_ui64(bytes,pos); pos+=8;
		bins=tickscounter.readbins(bytes[pos:]);
		t=tickscounter.tickscounter(epoch_at_start,millis_at_end,bins,r.time);
		Q.append(t);
	return "\n".join([str(q) for q in Q]);	

	
def main():
	sql=data.Sql();
	print(get_tickscounter(sql,10));

if __name__ == "__main__":
	print("main");
	main();

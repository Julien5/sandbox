#!/usr/bin/env python3

import data;
import sys;
sys.path.append("deserialization")
import tickscounter;

from readints import *;

sql=data.Sql();
R=sql.read_request("/compteur/tickcounter/data");
for r in R:
	bytes=r.data;
	pos=0;
	index=0;
	bins=list();
	epoch_at_start=read_ui64(bytes,pos); pos+=8;
	millis_at_end=read_ui64(bytes,pos); pos+=8;
	bins=tickscounter.readbins(bytes[pos:]);
	t=tickscounter.tickscounter(epoch_at_start,millis_at_end,bins);
	print(t);

	#tickscounter();
	

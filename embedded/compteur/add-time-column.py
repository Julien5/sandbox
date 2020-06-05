#!/usr/bin/env
import sys;

t=0;
f=open("/tmp/data.csv",'r');
period=int(sys.argv[1]);
for line in f.readlines():
    e=line.strip();
    if not e:
        continue;
    x=int(e);
    print(t,x);
    t+=period;
    

#!/usr/bin/env python3

import sys;

def chunks(data,L):
    ret=list();
    for n in range(int(len(data)/L)):
        c = data[n*L:(n+1)*L];
        ret.append(c);
    return ret;

def stringify(A):
    if type(A) is list:
        return "\t".join([str(a) for a in A]);
    return str(A);

def write(A,filename):
    f=open(filename,'w');
    f.write("\n".join([stringify(a) for a in A]));

def read_hex(filename,sampling_period_ms):
    ret=list();
    t=0;
    for hexline in open(filename,'r').readlines():
        D4=chunks(hexline.strip(),4);
        for d4 in D4:
            assert(len(d4)==4);
            c2 = chunks(d4,2);
            assert(len(c2)==2);
            x = int("".join(c2[::-1]), 16)
            if t%50 == 0:
                ret.append([t,x]);
            t+=sampling_period_ms;
    return ret;

period=int(sys.argv[1]);
write(read_hex("/tmp/irdata",period),'data.csv');

#!/usr/bin/env python3

def chunks(data,L):
    ret=list();
    for n in range(int(len(data)/L)):
        ret.append(data[n*L:(n+1)*L-1]);
    return ret;

def read(filename):
    D8=chunks(open(filename,'r').read(),8);
    for d8 in D8:
        c2 = chunks(d8,2);
        number = int("".join(c2[::-1]), 16)
        print(number);

read("/tmp/irdata3");

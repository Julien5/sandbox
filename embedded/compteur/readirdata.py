#!/usr/bin/env python3

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

xprev = None;
yprev = None;
def filter(x):
    global xprev,yprev;
    if not xprev:
        xprev = x;
        yprev = x;
    #if x < 40:
    #    x=xprev;
    alpha=0.5;
    y=alpha*yprev + (1-alpha)*x;
    xprev=x;
    yprev=y;
    return y;

def read_hex(filename):
    ret=list();
    t=0;
    for hexline in open(filename,'r').readlines():
        D4=chunks(hexline.strip(),4);
        for d4 in D4:
            assert(len(d4)==4);
            c2 = chunks(d4,2);
            assert(len(c2)==2);
            x = int("".join(c2[::-1]), 16)
            ret.append([t/1000,x,filter(x)]);
            t+=50;
    return ret;
        
write(read_hex("/tmp/irdata"),'data.csv');

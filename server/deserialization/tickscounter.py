#!/usr/bin/env python3

####
#Clock::ms m_start=0; 4 bytes
#count m_count=0; 2 bytes
#duration m_duration=0; 4 bytes
###
import datetime
from readints import *;

class bin:
    def __init__(self,start,count,duration):
        self.start = start;
        self.count = count;
        self.duration = duration;

    def end(self):
        return self.start + self.duration;
        
    def __str__(self):
        return "%9d->%-9d [%6d] #=%3d" % (self.start/1000,self.end()/1000,self.duration/1000,self.count);

class tickscounter:
    def __init__(self,start,end,bins):
        self.epoch_at_start = datetime.datetime.fromtimestamp(start/1000);
        self.millis_at_end = end;
        self.bins = bins;

    def __str__(self):
        s="start:%s millis(end)=%d" % (self.epoch_at_start.strftime("%Y-%m-%d %H:%M:%S"),self.millis_at_end);
        b="\n".join([str(b ) for b in self.bins if b.count > 1]);
        return s+"\n"+b;
    
def readbins(bytes):
    pos=0;
    index=0;
    ret=list();
    while pos<len(bytes):
        start=read_ui32(bytes,pos); pos+=4;
        count=read_ui16(bytes,pos); pos+=2;
        duration=read_ui32(bytes,pos); pos+=4;
        end=start+duration;
        index+=1;
        ret.append(bin(start,count,duration));
    return ret;

def main():
    f=open("tickscounter.packed.bin",'rb');
    bytes=f.read();
    pos=0;
    index=0;
    B=readbins(bytes);
    for index in range(len(B)):
        b=B[index];
        print("%02d: %s" % (index,str(b)));
        index+=1;

if __name__ == "__main__":
    main();

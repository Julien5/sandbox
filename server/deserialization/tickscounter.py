#!/usr/bin/env python3

####
#Clock::ms m_start=0; 4 bytes
#count m_count=0; 2 bytes
#duration m_duration=0; 4 bytes
###
import datetime
from readints import *;

class raw_bin:
    def __init__(self,start,count,duration):
        self.start = start;
        self.count = count;
        self.duration = duration;

    def end(self):
        return self.start + self.duration;
        
    def __str__(self):
        return "%9d->%-9d [%6d] #=%3d" % (self.start/1000,self.end()/1000,self.duration/1000,self.count);

class bin:
    def __init__(self,reset_time,rbin):
        self.start = reset_time+datetime.timedelta(milliseconds=rbin.start);
        self.count = rbin.count;
        self.duration = datetime.timedelta(rbin.duration);

    def end(self):
        return self.start + self.duration;
        
    def __str__(self):
        t1=self.start.strftime("%H:%M:%S");
        t2=self.end().strftime("%H:%M:%S");
        return "%s -> %s #=%3d" % (t1,t2,self.count);
    

class tickscounter:
    def __init__(self,start,millis_at_end,raw_bins,end):
        assert(raw_bins);
        milli_1=raw_bins[0].start;
        self.epoch_1=datetime.datetime.fromtimestamp(start/1000);
        assert(milli_1>=0)
        epoch_0=self.epoch_1 - datetime.timedelta(milliseconds=milli_1);
        assert(epoch_0<=self.epoch_1)
        self.bins=[];
        for b in raw_bins:
            c=bin(epoch_0,b);
            if c.count:
                self.bins.append(c);
        assert(not self.start() or self.start()<=self.end())

    def start(self):
        if not self.bins:
            return None;
        return self.bins[0].start;

    def end(self):
        if not self.bins:
            return None;
        return self.bins[-1].end();
        
    def total_millis(self):
        return (self.end() - self.start()).total_seconds()*1000;

    def __str__(self):
        if not self.bins:
            return "start:%s {} " % (self.epoch_1.strftime("%Y-%m-%d %H:%M:%S"))
        s="start:%s end=%s" % (self.start().strftime("%Y-%m-%d %H:%M:%S"),self.end().strftime("%Y-%m-%d %H:%M:%S"));
        b="\n".join([str(b) for b in self.bins if b.count > 0]);
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
        ret.append(raw_bin(start,count,duration));
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

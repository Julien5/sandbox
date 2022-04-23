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
    def __init__(self,start,count,duration):
        self.start = start;
        self.count = count;
        self.duration = duration;

    @classmethod
    def from_raw_bin(cls,reset_time,rbin):
        start = reset_time+datetime.timedelta(milliseconds=rbin.start);
        count = rbin.count;
        duration = datetime.timedelta(milliseconds=rbin.duration);
        return cls(start,count,duration);

    def end(self):
        return self.start + self.duration;

    def midbin(self,start,end):
        if not (self.end()>=start and self.start<=end):
            return None;
        s=max(self.start,start);
        e=min(self.end(),end);
        assert(s<=e);
        c=self.count;
        if self.duration.total_seconds() > 0:
            dloc = e-s;
            proportion=dloc/self.duration;
            c=proportion*self.count;
        return bin(s,c,e-s);

    def ticks(self):
        if self.count == 0:
            return list();
        if self.count == 1:
            return [self.start];
        t1=self.start
        t2=self.end();
        N=self.count
        delta=(t2-t1)/(N-1);
        T=[t1+k*delta for k in range(N)];
        assert(len(T)==N)
        assert(T[0]==t1);
        assert(abs(T[-1]-t2).total_seconds()<1);
        return T;

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
            c=bin.from_raw_bin(epoch_0,b);
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

    def getbins(self):
        return self.bins;
        
    def total_millis(self):
        return (self.end() - self.start()).total_seconds()*1000;

    def __str__(self):
        if not self.bins:
            return "start:%s {} " % (self.epoch_1.strftime("%Y-%m-%d %H:%M:%S"))
        s="start:%s end=%s" % (self.start().strftime("%Y-%m-%d %H:%M:%S"),self.end().strftime("%Y-%m-%d %H:%M:%S"));
        b="\n".join([str(b) for b in self.bins if b.count > 0]);
        return s+"\n"+b;

    def count(self):
        return sum([b.count for b in self.bins]);

    
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

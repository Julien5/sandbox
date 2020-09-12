#!/usr/bin/env python3

####
#  struct Bin {
#    uint16_t value=0;
#    uint32_t count=0;
#  } __attribute__((packed));
###  

class bin:
    def __init__(self,value,count):
        self.value = value;
        self.count = count;
      
    def __str__(self):
        return "[%2d]=%3d" % (self.value,self.count);

class Histogram:
    def __init__(self,bins):
        self.bins = bins;
        
    def count(self):
        return sum([b.count for b in self.bins if b.count>0]);

    def __str__(self):
        ret=list();
        for index in range(len(self.bins)):
            b=self.bins[index];
            ret.append("%02d: %s" % (index,str(b)));
            index+=1;
        return "\n".join(ret);
    
def read_ui32(bytes,pos):
    return int.from_bytes(bytes[pos:pos+4], byteorder='little', signed=True)

def read_ui16(bytes,pos):
    return int.from_bytes(bytes[pos:pos+2], byteorder='little', signed=True)
    
def readhistogram(bytes):
    pos=0;
    index=0;
    ret=list();
    while pos<len(bytes):
        value=read_ui16(bytes,pos); pos+=2;
        count=read_ui32(bytes,pos); pos+=4;
        index+=1;
        ret.append(bin(value,count));
    return Histogram(ret);


def main():
    #b=open("histogram.packed.bin",'rb').read();
    b=open("histogram.packed.hex",'rb').read().decode("utf-8");
    for h in b.split("\n"):
        b=bytes.fromhex(h);
        pos=0;
        index=0;
        H=readhistogram(b);
        print(str(H));
        print("count:",H.count());

if __name__ == "__main__":
    main();

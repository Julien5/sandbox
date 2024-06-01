#!/usr/bin/env python3

import exifread;
import os;
import hashlib;
import datetime;

class ExifData:
    def __init__(self,D):
        self.D=dict();
        for tag in D.keys():
            self.D[tag]=str(D[tag]);
        # set date and time
        self.datetime=None;
        dts=str(D["EXIF DateTimeOriginal"]);
        # 2011:12:24 20:44:12
        dt = datetime.datetime.strptime(dts, "%Y:%m:%d %H:%M:%S");
        self.datetime=dt;
        self.datetimehash=int(self.datetime.strftime("%Y%m%d%H%M%S"));
        
    def print(self):
        for tag in self.D.keys():
            if "EXIF " in tag or "Image " in tag:
                print(f"{tag:s}: {self.D[tag]:s}");

    def subpath(self):
        return self.datetime.strftime("%Y/%m/%d/%H-%M-%S")

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False;
        H1=self.__hash__();
        H2=other.__hash__();
        return H1 == H2;
                    
    def __hash__(self):
        L=list();
        for tag in self.D.keys():
            L.append(self.D[tag]);
        return sum([int(hashlib.sha1(s.encode("utf-8")).hexdigest(), 16) for s in L]);

def exifstat(filename):
    with open(filename, 'rb') as jpgfile:
        tags = exifread.process_file(jpgfile,stop_tag="DateTimeOriginal");
        print(filename);
        ExifData(tags).print();

def exifstats(L):
    for filename in L:
        exifstat(filename);

def exifcheck(L):
    ExifSet=set();
    SizeSet=set();
    for filename in L:
        with open(filename, 'rb') as jpgfile:
            tags = exifread.process_file(jpgfile);
            ExifSet.add(ExifData(tags));
            SizeSet.add(os.path.getsize(filename));
    assert(len(SizeSet)==1);
    if len(ExifSet) != len(L):
        print(f"{len(ExifSet):d}-exif uniq elements for {len(L):d} files:")
        print("\n".join(L));
        #exifstat(L);

class List:
    def __init__(self):
        self.D = dict();

    def append(self,filename,size):
        if not size in self.D:
            self.D[size]=list();
        self.D[size].append(filename);

    def exifchecks(self):
        for size in self.D:
            L=self.D[size];
            if len(L)>1:
                exifcheck(L);
        
    def stats(self):
        R=dict();
        # 1995840
        for size in self.D:
            Lsize=self.D[size];
            if not len(Lsize) in R:
                R[len(Lsize)] = 0;
            R[len(Lsize)] += 1;
        for n in sorted(R.keys()):
            print(f"number of {n:d}-duplicates: {R[n]:d}");

        
def main():
    filename="/home/julien/tmp/tmp/lists/all.extended";
    f=open(filename,'r');
    lines=f.read().split("\n");
    L=List();
    print("reading..");
    for line in lines:
        if not "|" in line:
            continue;
        parts=line.split("|");
        size=int(parts[1]);
        filename=parts[3];
        L.append(filename,size);
    exifstat("/media/julien/backup/home/julien/DCIM/100OLYMP/PC240191.JPG");
    return;
    L.stats();
    L.exifchecks();
    return 1

if __name__ == "__main__":
   main();

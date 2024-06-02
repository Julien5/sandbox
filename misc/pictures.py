#!/usr/bin/env python3

import exifread;
import os;
import hashlib;
import datetime;
import json;

def readdatetimefromexif(D):
    tags=D.keys();
    if "EXIF DateTimeOriginal" in tags:
        dts=str(D["EXIF DateTimeOriginal"]);
        # 2011:12:24 20:44:12
        if "0000" in dts:
            #assert(D['Image Make']=="Panasonic");
            #assert(D['Image Model']=='DMC-FX01');
            return None;
        try:
            return datetime.datetime.strptime(dts, "%Y:%m:%d %H:%M:%S");
        except Exception as e:
            print("could not read date time from",dts,e);
            pass;
        
    for t in tags:
        short=shorttag(t);
        if "time" in short and "date" in short:
            dts=str(D[t]);
            try:
                return datetime.datetime.strptime(dts, "%Y:%m:%d %H:%M:%S");
            except Exception as e:
                print("could not read date time from",dts,e);
                pass;
    return None;

def readdatetimefromfilename(filename):
    return datetime.datetime.fromtimestamp(int(os.path.getmtime(filename)));

def readdatetime(exif,filename):
    E=readdatetimefromexif(exif);
    if E:
        return E;
    if exif and not "30x-backup" in filename:
        pass;
        #assert(exif['Image Make']=="Panasonic");
        #assert(exif['Image Model']=='DMC-FX01');
    return readdatetimefromfilename(filename);

def find_exif(exif,part):
    for tag in exif:
        if part in tag.lower():
            return exif[tag];
    return None;    

def md5(filename):
    hash_md5 = hashlib.md5()
    with open(filename, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

def shorttag(tag):
    return tag.replace(" ","").lower();

class Data:
    def __init__(self,filename):
        self.exif=dict();
        with open(filename, 'rb') as jpgfile:
            tags = exifread.process_file(jpgfile,stop_tag="DateTimeOriginal");
            for tag in tags:
                value=str(tags[tag]);
                if len(value)<32:
                    self.exif[tag]=str(tags[tag]);
            # set date and time
        self.datetime=readdatetime(self.exif,filename);
        self.filename=filename;
        self.size=os.path.getsize(self.filename);
        
    def print(self):
        for tag in self.exif.keys():
            if "EXIF " in tag or "Image " in tag:
                print(f"{tag:s}: {self.exif[tag]:s}");
                
    def subpath(self):
        D="unknown";
        b1="";
        if self.exif:
            if readdatetimefromexif(self.exif):
                D=self.datetime.strftime("%Y/%m/%d");
                b1=self.datetime.strftime("%H-%M-%S");
            else:
                manufacturer=find_exif(self.exif,"manufacturer");
                if not manufacturer:
                    manufacturer=find_exif(self.exif,"make");
                #model=find_exif(self.exif,"model");
                if manufacturer:
                    D=f"{manufacturer:s}";
        parts=os.path.basename(self.filename).split(".");
        # f9f1586944c07678879c718319dd266b.P1080052.JPG
        # capture original basename
        b2=parts[0];
        if len(parts)>2 and len(parts[0])>30:
            b2=parts[1];
        extension=os.path.basename(self.filename).split(".")[-1];
        if b1:
            return f"{D:s}/{b1:s}-{b2:s}.{extension:s}";
        return f"{D:s}/{b2:s}.{extension:s}";

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False;
        H1=self.__hash__();
        H2=other.__hash__();
        return H1 == H2;

    def datetimehash(self):
        return int(self.datetime.strftime("%Y%m%d%H%M%S"));
                    
    def __hash__(self):
        return self.datetimehash();
        L=list();
        for tag in self.exif.keys():
            L.append(self.exif[tag]);
        return sum([int(hashlib.sha1(s.encode("utf-8")).hexdigest(), 16) for s in L]);

    def get_exif(self):
        return self.exif;

def collect(filename):
    print("collect",filename);
    ret=dict();
    #ret["md5sum"]=md5(filename);
    ret["exif"]=createExifData(filename).get_exif();
    ret["mtime"]=int(os.path.getmtime(filename));
    ret["size"]=os.path.getsize(filename);
    ret["filename"]=filename;
    return ret;

def checkmd5eq(L):
    assert(L);
    return len(set([md5(filename) for filename in L]))==1;

def md5uniqs(L):
    assert(L);
    ret=dict();
    if len(L)<2:
        assert(type(L)==type(list()));
        return [L[0]];
    for filename in L:
        assert(filename != "/");
        ret[md5(filename)]=filename;
    return list(ret.values());
    
def exifstat(filename):
    createExifData(filename).print();

def createExifDataWorker(filename):
    with open(filename, 'rb') as jpgfile:
        return Data(filename);

def createExifData(filename):
    try:
        return createExifDataWorker(filename);
    except Exception as e:
        print("error with",filename,e);
        raise e;
    return None;

def exifstats(L):
    for filename in L:
        exifstat(filename);

def exifcheck(L):
    # all filenames in L have the same size
    ExifSet=dict();
    SizeSet=set();
    for filename in L:
        SizeSet.add(os.path.getsize(filename));
        e=createExifData(filename);
        if not e:
            print("ignore",filename);
            continue;
        if not e in ExifSet:
            ExifSet[e]=list();
        ExifSet[e].append(filename);

    assert(len(SizeSet)==1);
    # return the representants
    ret=list();
    for e in ExifSet.keys():
        U=md5uniqs(ExifSet[e]);
        ret.extend(U);
    assert(ret);
    if len(L) != len(ret):
        print(f"{len(ret):d} uniq elements for {len(L):d} files:")
        print("in","\n".join(L))
        print("out","\n".join(ret))
        print("--");
    return ret;

class List:
    def __init__(self):
        self.D = dict();

    def append(self,filename,size):
        if not size in self.D:
            self.D[size]=list();
        self.D[size].append(filename);

    def siblings(self,filename):
        # same size, same basename
        size=os.path.getsize(filename);
        assert(size in self.D);
        ret=list();
        for o in D[size]:
            if os.path.basename(filename) == os.path.basename(o):
                ret.append(o);
        return ret;   

    def exifchecks(self):
        L=list();
        for size in self.D:
            l=self.D[size];
            Lloc=exifcheck(l);
            L.extend(Lloc);
        return L;

    def stats(self):
        R=dict();
        # 1995840
        source_size=0;
        for size in self.D:
            Lsize=self.D[size];
            source_size += size*len(Lsize);
            if not len(Lsize) in R:
                R[len(Lsize)] = 0;
            R[len(Lsize)] += 1;
        for n in sorted(R.keys()):
            print(f"number of {n:d}-duplicates: {R[n]:d}");
        print("source size",source_size);
        
def main():
    L=List();
    print("reading..");
    #filename="/home/julien/tmp/tmp/lists/all.extended";
    filename="/tmp/all.extended";
    f=open(filename,'r');
    lines=f.read().split("\n");
    for line in lines:
        if not "|" in line:
            continue;
        parts=line.split("|");
        size=int(parts[1]);
        filename=parts[3];
        assert(len(filename)>3);
        L.append(filename,size);
    L.stats();
    # uniq
    U=L.exifchecks();
    f=open("rename_dict.txt",'w');
    target_size=0;
    for u in U:
        assert(u != "/");
        D=createExifData(u);
        subpath=D.subpath()
        #print(f"{u:s} -> /{subpath:s}");
        f.write(f"{u:s}|/{subpath:s}\n");
        target_size += D.size;
    f.close();
    L.stats();
    print("target size",target_size);
if __name__ == "__main__":
   main();

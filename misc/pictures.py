#!/usr/bin/env python3

import exifread;
import os;
import hashlib;
import datetime;

def shorttag(tag):
    return tag.replace(" ","").lower();

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

def hash_dict(D):
    hash_md5 = hashlib.md5()
    for k in D.keys():
        hash_md5.update(k.encode("utf-8"));
        hash_md5.update(D[k].encode("utf-8"));
    return int.from_bytes(hash_md5.digest(),'big');

hard_md5=0;
def computemd5(filename):
    global hard_md5;
    hard_md5 += 1;
    hash_md5 = hashlib.md5()
    with open(filename, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

hard_exif=0;
def computeexif(filename):
    global hard_exif;
    hard_exif += 1;
    with open(filename, 'rb') as jpgfile:
        #tags = exifread.process_file(jpgfile,stop_tag="DateTimeOriginal");
        tags = exifread.process_file(jpgfile,details=False);
        _exif=dict();
        for tag in tags:
            value=str(tags[tag]);
            if len(value)<32:
                _exif[tag]=str(tags[tag]);
        return _exif;
                

class Image:
    def __init__(self,filename):
         self.filename=filename;
         self._md5=None;
         self._exif=None;

    def exif(self):
        if self._exif is None:
            self._exif=computeexif(self.filename);
        return self._exif;

    def md5sum(self):
        if self._md5 is None:
            self._md5=computemd5(self.filename);
        return self._md5
       
    def size(self):
        return os.path.getsize(self.filename);

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False;
        if self.size() != other.size():
            return False;
        if self.exif():
            return self.exif() == other.exif();
        if os.path.basename(self.filename) == os.path.basename(other.filename):
            return True;
        #print(f"hard compare: {self.filename:s} to {other.filename:s}",end="");
        if self.md5sum() != other.md5sum():
            #print(" [differ]");
            return False;
        #print(" [equal]");
        return True;

    def __hash__(self):
        return self.size();

    def print(self):
        for tag in self.exif.keys():
            if "EXIF " in tag or "Image " in tag:
                print(f"{tag:s}: {self.exif[tag]:s}");
                
    def subpath(self):
        D="unknown";
        b1="";
        if self.exif:
            dt=readdatetimefromexif(self.exif())
            if dt:
                D=dt.strftime("%Y/%m/%d");
                b1=dt.strftime("%H-%M-%S");
            else:
                manufacturer=find_exif(self.exif(),"manufacturer");
                if not manufacturer:
                    manufacturer=find_exif(self.exif(),"make");
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

def exifstat(filename):
    createImage(filename).print();

def createImageWorker(filename):
    return Image(filename);

def createImage(filename):
    try:
        return createImageWorker(filename);
    except Exception as e:
        print("error with",filename,e);
        raise e;
    return None;

class List:
    def __init__(self):
        self.D = dict();
        self.ninput=0;

    def append(self,filename):
        image=Image(filename);
        if not image in self.D:
            self.D[image]=list();
        self.D[image].append(image);
        self.ninput+=1;
        if self.ninput % 250 == 0:
            r=len(self.D)/self.ninput;
            print(f"processed {self.ninput:d} files to {len(self.D):5d} images [{100*r:3.1f}%]");

    def uniqs(self):
        U=list();
        for image in self.D:
            U.append(image.filename);
        return U;

    def stats(self):
        global hard_exif;
        global hard_md5;
        R=dict();
        source_size=0;
        target_size=0;
        source_nfiles=0;
        target_nfiles=len(self.D);
        for image in self.D:
            source_size += sum([i.size() for i in self.D[image]]);
            source_nfiles += len(self.D[image]);
            target_size += image.size();
        print("source nfiles:",source_nfiles);
        print("target nfiles:",target_nfiles);            
        print("source size:",source_size);
        print("target size:",target_size);
        print("hard exif:",hard_exif);
        print("hard md5:",hard_md5);

def makeList():
    L=List();
    print("reading..");
    #filename="/home/julien/tmp/tmp/lists/all.extended";
    filename="/tmp/all.extended";
    f=open(filename,'r');
    lines=f.read().split("\n");
    f.close();
    idict=dict();
    for line in lines:
        if not "|" in line:
            continue;
        parts=line.split("|");
        size=int(parts[1]);
        filename=parts[3];
        L.append(filename);
    return L;    
        
def main():
    L=makeList();
    L.stats();
    print("collecting representants");
    U=L.uniqs();
    #f=open("rename_dict.txt",'w');
    #print("writing representants (this may take a while)");
    #lines=[f"{u:s}|{createImage(u).subpath():s}" for u in U];
    #f.write("\n".join(lines));
    #f.close();
    
if __name__ == "__main__":
   main();


   

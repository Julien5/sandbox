#!/usr/bin/env python3

import hamster
import sys;
import os;
import sqlite3;

sys.path.append("~/home/julien/.local/lib/python3.6/site-packages/");
import statistics;
import datetime;
import json;
import subprocess;
import binascii;

def output_name(filename):
    return filename.replace(".csv",".png");

def update_plot(filename,D=None):
    print("updating:",filename);
    script=None;
    if "millis" in filename:
        script=open("millis.gnuplot").read();
    if "minutes" in filename:
        script=open("minutes.gnuplot").read();
    if not script:
        return None;
    script = script.replace("{date}",os.path.basename(filename).split(".")[0]);
    script = script.replace("{in}",filename);
    script = script.replace("{out}",output_name(filename));
    if D:
        for d in D:
            script = script.replace("{"+d+"}",str(D[d]));    
    open("plot.gnuplot",'w').write(script);
    return subprocess.call(["gnuplot", "plot.gnuplot"]);

def update_file(filename,content,D=None):
    if os.path.exists(filename) and os.path.exists(output_name(filename)):
        oldcontent=str(open(filename,'r').read());
        if oldcontent == content:
            print("skip:",filename);
            return False;
    f=open(filename,'w');
    f.write(content);
    f.flush();
    update_plot(filename,D);
    return True;

def minutes_csv(minutes):
    d = str();
    for m in sorted(minutes):
        minute = 60*m.hour + m.minute;
        d += str(minute)+","+str(minutes[m])+"\n";
    return d;

class Bin:
    def __init__(self,start,duration,count,transmit_time):
        self.start = start;
        self.duration = duration;
        self.count = count;
        self.transmit_time = transmit_time;
     
    def end(self):
        return self.start + self.duration;
    
    def string(self):
        tr=self.transmit_time.strftime("%Y-%m-%d-%H:%M");
        st=self.start.strftime("%d-%H:%M");
        du=int(self.duration.total_seconds());
        return "transmit:{} start:{} duration:{:3d} count:{:4d}".format(tr,st,du,self.count);
        
class Ticks:
    def __init__(self,bins):
        assert(isinstance(bins,list));
        self.bins = bins;  

    def append(self,b):
        self.bins.append(b);
            
    def total(self):
        ret=0;
        for b in self.bins:
            ret+=b.count;
        return ret;

    def duration(self):
        ret=None;
        for b in self.bins:
            if not ret:
                ret=b.duration;
            else:
                ret+=b.duration;
        return ret;

    def string(self):
        lines=[];
        for b in self.bins:
            lines.append(b.string());
        return "\n".join(lines);

    def interval(self,start,end):
        B=list();
        for b in self.bins:
            if b.end() >= start and b.start <= end:
                B.append(b);
        if not B:
            return None;
        return Ticks(B);
                    
class TicksHandler:
    def __init__(self, ticks):
        self.ticks = ticks;
   
    def total(self):
        return self.ticks.total();

    def duration(self):
        return self.ticks.duration();

    def sms(self):
        end = datetime.datetime.now();
        start = end - datetime.timedelta(hours=24);
        T=self.ticks.interval(start,end);
        if not T:
            return "empty.";
        return "24: T={0} | {1}m".format(T.total(),int(T.duration().total_seconds()/60));

    def _stats(self,start,end):
        T=self.ticks.interval(start,end);
        if not T:
            return None;
        ret={};
        ret['date'] = end.strftime("%Y-%m-%d");
        ret['count']=T.total();
        ret['duration']=int(T.duration().total_seconds()/60);
        ret['#bins']=len(T.bins);
        return ret;

    def stats(self):
        ret=[];
        end = datetime.datetime.now();
        while True:
            start = end - datetime.timedelta(hours=24);
            s=self._stats(start,end);
            if not s:
                break;
            ret.append(str(s));
            end=start;
        return "\n".join(ret);
            
    def string(self):
        return self.ticks.string();

def _bins(jstring,tT):
    bins = list();
    D=json.loads(jstring);
    mT=int(D["transmit_time"]);
    for b in D["bins"]:
        m=int(b["start"]);
        delta=datetime.timedelta(milliseconds=(mT-m));
        start=tT-delta;
        duration=datetime.timedelta(milliseconds=int(b["duration"]));
        count=int(b["count"]);
        bins.append(Bin(start,duration,count,tT));
    return bins;
        
def to_datetime(s):
    return datetime.datetime.strptime(s,"%Y-%m-%d %H:%M:%S.%f");

def ticks_from_post_data(data,t):
    tT=to_datetime(t);
    hx=binascii.hexlify(data).decode('ascii');
    json=hamster.tickscounter.asJson(hx);
    return Ticks(_bins(json,tT));
    
def try_create(sqlite,stm):
    try:
        sqlite.execute(stm);
    except sqlite3.OperationalError as e:
        pass; # print("could not run:",stm);
      
class Sql:
    def __init__(self):
        create = not os.path.exists("sqlite.db");
        self.conn = sqlite3.connect("sqlite.db");
        self.sqlite = self.conn.cursor();
        try_create(self.sqlite,"CREATE TABLE _requests (ID INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT, data BLOB, time TEXT)");
        try_create(self.sqlite,"CREATE TABLE ticks (ID INTEGER PRIMARY KEY AUTOINCREMENT,  start TEXT, duration_ms INTEGER, count INTEGER, transmit TEXT)");
        self.conn.commit();
        
    def insert_request(self,path,data):
        t=str(datetime.datetime.now());
        self.sqlite.execute('INSERT INTO _requests (path,data,time) VALUES (?,?,?)', (path, data, t));
        if data:
            self.insert_ticks(ticks_from_post_data(data,t)); 
        self.conn.commit();
    def _insert_bin(self,b):
        start = b.start;
        duration_ms = int(b.duration/datetime.timedelta(milliseconds=1));
        count = b.count;
        transmit = b.transmit_time;
        self.sqlite.execute('INSERT INTO ticks (start,duration_ms,count,transmit) VALUES (?,?,?,?)', (start,duration_ms,count,transmit));
        
    def insert_ticks(self,ticks):
        for b in ticks.bins:
            self._insert_bin(b);
        self.conn.commit();

    def exe(self,statement):
        self.sqlite.execute(statement);
        if "select" in statement.lower():
            return self.sqlite.fetchall();
        return None;  
    
    def select_ticks(self):
        bins=list();
        for row in self.exe("SELECT start,duration_ms,count,transmit FROM ticks"):
            (_start,_duration_ms,count,_transmit)=row;
            start = to_datetime(_start);
            transmit = to_datetime(_transmit);
            assert(isinstance(start,datetime.datetime));
            duration = datetime.timedelta(milliseconds=_duration_ms);
            bins.append(Bin(start,duration,count,transmit));
        return Ticks(bins);            
                  
# update = reset + rebuild all => really not efficient
def rebuild():
    sql=Sql();
    sql.exe("DELETE FROM ticks;");
    for row in sql.exe("SELECT path,data,time FROM _requests"):
        (path,data,t)=row;
        if "tickscounter" in path:
            assert(isinstance(data,bytes));
            sql.insert_ticks(ticks_from_post_data(data,t));
        
if __name__ == "__main__":
    #update_plot("minutes/2019-01-22.csv");
    #update_plot("millis/2019-01-19-18:7-0.csv");
    rebuild();
    ticks=TicksHandler(Sql().select_ticks());
    print("stats:",ticks.stats());
    print("sms:",ticks.sms());

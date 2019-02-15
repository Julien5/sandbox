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

class Tick:
    def __init__(self,start,duration,count,transmit_time):
        self.start = start;
        self.duration = duration;
        self.count = count;
        self.transmit_time = transmit_time;
        
    def end(self):
        return self.start + self.duration;
    
    def string(self):
        return "start:{} duration:{:3d} count:{:4d} transmit:{}".format(self.start.strftime("%Y-%m-%d-%H:%M"),
                                                         int(self.duration.total_seconds()),
                                                         self.count,
                                                         str(self.transmit_time.strftime("%Y-%m-%d-%H:%M")));
        
class Ticks:
    def __init__(self):
        self.bins = list();
     
    def append(self,jstring,tT):
        D=json.loads(jstring);
        print("process:",D);        
        mT=int(D["transmit_time"]);
        for b in D["bins"]:
            m=int(b["start"]);
            delta=datetime.timedelta(milliseconds=(mT-m));
            start=tT-delta;
            duration=datetime.timedelta(milliseconds=int(b["duration"]));
            count=int(b["count"]);
            self.bins.append(Tick(start,duration,count,tT));
            
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
            
class Data:
    def __init__(self):
        self.ticks = Ticks();

    def reset(self):
        self.ticks = Ticks();

    def merge(self,t,json):
        self.ticks.append(json,t);
           
    def process(self,hex,t):
        t=datetime.datetime.strptime(t,"%Y-%m-%d %H:%M:%S.%f");
        self.merge(t,hamster.tickscounter.asJson(hex));

    def total(self):
        return self.ticks.total();

    def duration(self):
        return self.ticks.duration();

    def sms(self):
        seconds=0;
        if self.duration():
            seconds=self.duration().total_seconds();
        return "T={0} | {1}s".format(self.total(),int(seconds));
        
    def string(self):
        return self.ticks.string();
            
class Sql:
    def __init__(self):
        create = not os.path.exists("sqlite.db");
        self.conn = sqlite3.connect("sqlite.db");
        self.sqlite = self.conn.cursor();
        if create:
            self.sqlite.execute("CREATE TABLE requests (ID INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT, data BLOB, time TEXT)");
            self.conn.commit();
    def select(self,statement):
        self.sqlite.execute(statement)
        return self.sqlite.fetchall();
    def insert(self,path,data):
        t=str(datetime.datetime.now());
        self.sqlite.execute('INSERT INTO requests (path,data,time) VALUES (?,?,?)', (path, data, t));
        self.conn.commit();

# update = reset + rebuild all => really not efficient
def update(sql,d):
    d.reset();
    for row in sql.select("SELECT path,data,time FROM requests"):
        (path,data,t)=row;
        if "tickscounter" in path:
            assert(isinstance(data,bytes));
            hx=binascii.hexlify(data).decode('ascii');
            d.process(hx,t);
        
def dump(sql,d):
    update(sql,d);
    print(d.string());
            
if __name__ == "__main__":
    #update_plot("minutes/2019-01-22.csv");
    #update_plot("millis/2019-01-19-18:7-0.csv");
    sql=Sql();
    d=Data();
    dump(sql,d);
    print("sms:",d.sms());

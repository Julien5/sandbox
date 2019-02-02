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

def millis_delta(millis):
    if len(millis)<5:
        return None;
    
    ret=list();
    m0=None;
    for m in millis:
        if not m:
            continue;
        if m0:
            ret.append(m-m0);
        m0 = m;
    return ret;

def millis_csv(millis):
    D=millis_delta(millis);
    i=0;
    d="";
    for delta in D:
        d += str(i)+","+str(delta);
        i = i + 1;
    return d;

# {
#  "bins":[{"start":0,"count":1,"duration":0}]
# }

class Tick:
    def __init__(self,start,duration,count):
        self.start = start;
        self.minduration = duration;
        self.count = count;
        
    def maxduration(self):
        return self.minduration + datetime.timedelta(minutes=1);

    def end(self):
        return self.start + self.maxduration();
    
    def string(self):
        return "start:{} end:{} count:{}".format(self.start.strftime("%Y-%m-%d-%H:%M"),
                                                 self.end().strftime("%Y-%m-%d-%H:%M"),
                                                 self.count);

class Ticks:
    def __init__(self,jstring,tT):
        D=json.loads(jstring);
        print("process:",D);        
        mT=int(D["transmit_time"]);
        self.bins = list();
        for b in D["bins"]:
            m=int(b["start"]);
            delta=datetime.timedelta(minutes=(mT-m));
            start=tT-delta;
            duration=datetime.timedelta(minutes=int(b["duration"]));
            count=int(b["count"]);
            self.bins.append(Tick(start,duration,count));
            
    def total(self):
        ret=0;
        for b in self.bins:
            ret+=b.count;
        return ret;

    def duration(self):
        ret=None;
        for b in self.bins:
            if not ret:
                ret=b.maxduration();
            else:
                ret+=b.maxduration();
        return ret;

    def display(self):
        for b in self.bins:
            print(b.string());
            
class Data:
    def __init__(self):
        self.ticks = list();

    def reset(self):
        self.ticks = list();

    def merge(self,t,json):
        self.ticks.append(Ticks(json,t));
           
    def process(self,hex,t):
        t=datetime.datetime.strptime(t,"%Y-%m-%d %H:%M:%S.%f");
        self.merge(t,hamster.tickscounter.asJson(hex));

    def total(self):
        T=0;
        for tick in self.ticks:
            T+=tick.total();
        return T;

    def duration(self):
        T=None;
        for tick in self.ticks:
            if not T:
                T=tick.duration();
            else:
                T+=tick.duration();
        return T;

    def sms(self):
        for tick in self.ticks:
            tick.display();
        seconds=0;
        if self.duration():
            seconds=self.duration().total_seconds();
        return "{0} ticks {1} min".format(self.total(),int(seconds/60));
  
    def dump(self):
        return;
        P = self.packets.packets();
        for date in P:
            filename = "dump/"+date.strftime("%Y-%m-%d--%H-%M-%S")+".csv";
            update_file(filename,minutes_csv(P[date].minutes));
        
        for date in self.packets.dates():
            start = datetime.datetime.combine(date,datetime.time(0,0,0));
            end   = datetime.datetime.combine(date,datetime.time(23,59,59));
            packets_d = self.packets.trunc(start,end);
            d = packets_d.csv("minutes");
            if not d:
                continue;
            filename = "minutes/"+date.strftime("%Y-%m-%d")+".csv";
            D=dict();
            D["nminutes"]=packets_d.number_of_minutes();
            D["ntours"]=packets_d.number_of_tours();
            update_file(filename,d,D);
            
        millis = self.packets.get_millis();
        for date in millis:
            m = millis[date];
            csv = millis_csv(m);
            if not csv:
                continue;
            filename = "millis/"+date.strftime("%Y-%m-%d--%H-%M-%S")+".csv";
            update_file(filename,csv);
            
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
    d.dump();
            
if __name__ == "__main__":
    #update_plot("minutes/2019-01-22.csv");
    #update_plot("millis/2019-01-19-18:7-0.csv");
    sql=Sql();
    d=Data();
    dump(sql,d);
    print("sms:",d.sms());

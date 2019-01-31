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

class Packet:
    def __init__(self,httpdate,jstring):
        self.minutes = dict();
        self.millis = list();
        self.date = httpdate;
        
        data=json.loads(jstring);
        m_http=httpdate.hour*60+httpdate.minute;
        D = data["minutes"];
        
        for m in D:
            m_stat = m["minute"];
            hr = int(m_stat/60);
            mn = int(m_stat-hr*60);
            date = httpdate.replace(hour=hr,minute=mn);
            if m_stat>m_http:
                date = date.replace(day=httpdate.date().day-1);
            self.minutes[date]=m["count"];
            
        for m in data["millis"]:
            self.millis.append(m);


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

class Packets:
    def __init__(self):
        self.minutes=dict();
        self.millis=dict();
        self.httpdates=dict()

    def merge(self,packet):
        if not packet.minutes:
            return;
        for m in packet.minutes:
            assert(not m in self.minutes);
            c=packet.minutes[m];
            if c>2:
                self.minutes[m] = c; 
        self.millis[sorted(packet.minutes)[-1]] = packet.millis;
        self.httpdates[packet.date]=packet;
        
    def dates(self):
        ret=set();
        for m in self.minutes:
            ret.add(m.date());
        return ret;

    def packets(self):
        return self.httpdates.copy();
        
    def get_millis(self):
        return self.millis.copy();
    
    def trunc(self,start,end):
        ret=Packets();
        for m in self.minutes:
            if start <= m and m <= end:
                ret.minutes[m] = self.minutes[m];
        for m in self.millis:
            if start <= m and m <= end:
                ret.millis[m] = self.millis[m];
        return ret;

    def number_of_tours(self):
        ret=0;
        for m in self.minutes:
            ret += self.minutes[m];
        return ret;

    def number_of_minutes(self):
        return len(self.minutes);

    def csv(self,key):
        d = str();
        if "minutes" in key:
            return minutes_csv(self.minutes);
        return d;

class Data:
    def __init__(self):
        self.packets=Packets();
        
    def merge(self,httpdate,jstring):
        self.packets.merge(Packet(httpdate,jstring));
           
    def process(self,hex,t):
        t1=datetime.datetime.strptime(t,"%Y-%m-%d %H:%M:%S.%f");
        t0=datetime.datetime.strptime("2019-01-19 18:00","%Y-%m-%d %H:%M");
        if not t1>t0:
            return;
        self.merge(t1,hamster.statistics.asJson(hex));

    def dump(self):
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

def update_all():    
    sql=Sql();
    d=Data();
    for row in sql.select("SELECT path,data,time FROM requests"):
        (path,data,t)=row;
        if "tickscounter" in path:
            assert(isinstance(data,bytes));
            hx=binascii.hexlify(req).decode('ascii');
            d.process(hx,t);
    d.dump();

if __name__ == "__main__":
    #update_plot("minutes/2019-01-22.csv");
    #update_plot("millis/2019-01-19-18:7-0.csv");
    update_all();

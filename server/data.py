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

def update_plot(filename):
    print("updating:",filename);
    script=None;
    if "millis" in filename:
        script=open("millis.gnuplot").read();
    if "minutes" in filename:
        script=open("minutes.gnuplot").read();
    assert(script);
    script = script.replace("{date}",os.path.basename(filename).split(".")[0]);
    script = script.replace("{in}",filename);
    script = script.replace("{out}",output_name(filename));
    open("plot.gnuplot",'w').write(script);
    return subprocess.call(["gnuplot", "plot.gnuplot"]);
    
class Data:
    def __init__(self):
        self.csv=dict();
        self.millis=dict();
        
    def merge(self,httpdate,jstring):
        data=json.loads(jstring);
        sent_time=httpdate.hour*60+httpdate.minute;
        back_minute=-1;
        for m in data["minutes"]:
            generated_time = m["minute"];
            back_minute = generated_time;
            generated_date = httpdate;
            if generated_time>sent_time:
                generated_date = httpdate.replace(day=httpdate.date().day-1);
            key="minutes/"+generated_date.strftime("%Y-%m-%d");
            if not key in self.csv:
                self.csv[key]=dict();
            self.csv[key][m["minute"]]=m["count"];
            
        h=int(back_minute/60);
        m=int(back_minute-h*60);
        key=httpdate.strftime("%Y-%m-%d")+"-"+str(h)+":"+str(m)+"-"+str(len(self.millis));
        self.millis[key]=list();
        m0=None;
        for m in data["millis"]:
            if m0 and m:
                self.millis[key].append(m0-m);
            m0=m;
            
    def process(self,hex,t):
        t1=datetime.datetime.strptime(t,"%Y-%m-%d %H:%M:%S.%f");
        t0=datetime.datetime.strptime("2019-01-19 18:00","%Y-%m-%d %H:%M");
        if not t1>t0:
            return;
        self.merge(t1,hamster.statistics.asJson(hex));

    def dump(self):
        for date in self.csv:
            d = "";
            for m in sorted(self.csv[date]):
                d += str(m)+","+str(self.csv[date][m])+"\n";
            filename=date+".csv";

            if not d:
                continue;

            if os.path.exists(filename) and os.path.exists(output_name(filename)):
                oldcontent=str(open(filename,'r').read());
                if oldcontent == d:
                    print("skip:",filename);
                    continue;

            f=open(filename,'w');
            f.write(d);
            f.flush();
            update_plot(filename);

        for k in self.millis:
            d = "";
            i=1;
            L=len(self.millis[k]);
            if L<5:
                continue;
            for m in self.millis[k]:
                d += str(i)+","+str(m)+"\n";
                i = i+1;

            if not d:
                continue;

            filename="millis/"+str(k)+".csv";
            if os.path.exists(filename) and os.path.exists(output_name(filename)):
                oldcontent=str(open(filename,'r').read());
                if oldcontent == d:
                    print("skip:",filename);
                    continue;
            f=open(filename,'w');
            f.write(d);
            f.flush();
            update_plot(filename);
            
class Sql:
    def __init__(self):
        self.conn = sqlite3.connect("sqlite.db");
        self.sqlite = self.conn.cursor();
    def select(self,statement):
        self.sqlite.execute(statement)
        return self.sqlite.fetchall();
    def insert(self,request):
        t=str(datetime.datetime.now());
        self.sqlite.execute('INSERT INTO requests (req,time) VALUES (?,?)', (request, t));
        self.conn.commit();

def update_all():    
    sql=Sql();
    data=Data();
    for row in sql.select("SELECT req,time FROM requests"):
        (req,t)=row;
        if isinstance(req,bytes):
            hx=binascii.hexlify(req).decode('ascii');
            data.process(hx,t);
    data.dump();

def insert(request,time):
    sql=Sql();
    

if __name__ == "__main__":
    #update_plot("minutes/2019-01-22.csv");
    #update_plot("millis/2019-01-19-18:7-0.csv");
    update_all();

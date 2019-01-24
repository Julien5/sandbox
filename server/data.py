#!/usr/bin/env python3

import hamster
import sys;
import os;
import sqlite3;

sys.path.append("~/home/julien/.local/lib/python3.6/site-packages/");
import statistics;
import datetime;
import json;

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
            key=generated_date.strftime("%Y-%m-%d");
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
            filename=date+".csv";
            f=open(filename,'w');
            d = "";
            for m in sorted(self.csv[date]):
                d += str(m)+","+str(self.csv[date][m])+"\n";
            f.write(d);
            
        for k in self.millis:
            filename="millis/"+str(k)+".csv";
            f=open(filename,'w');
            d = "";
            i=1;
            for m in self.millis[k]:
                d += str(i)+","+str(m)+"\n";
                i = i+1;
            f.write(d);

class Sql:
    def __init__(self,filename):
        self.conn = sqlite3.connect(filename);
        self.sqlite = self.conn.cursor();
    def select(self,statement):
        self.sqlite.execute(statement)
        return self.sqlite.fetchall();
    
def main():
    global csv;
    
    filename="sqlite.db";
    sql=Sql(filename);
    data=Data();
    for row in sql.select("SELECT req,time FROM requests"):
        (req,t)=row;
        if isinstance(req,bytes):
            data.process(req.hex(),t);
    data.dump();
    

if __name__ == "__main__":
    main();

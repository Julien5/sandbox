#!/usr/bin/env python3

import hamster
import sys;
import os;
import sqlite3;

sys.path.append("~/home/julien/.local/lib/python3.6/site-packages/");
import statistics;
import datetime;
import json;

conn=None;
sqlite=None;

csv=dict();

def merge(t,j):
    global csv;
    date=t;
    data=json.loads(j);
    d="";
    h="";
    sent_time=t.hour*60+t.minute;
    for m in data["minutes"]:
        # d += str(m["minute"])+","+str(m["count"])+"\n";
        generated_time = m["minute"];
        generated_date = date;
        if generated_time>sent_time:
            generated_date = date.replace(day=date.date().day-1);
            #print(m["minute"],date.strftime("%Y-%m-%d"),"->",generated_date.strftime("%Y-%m-%d"));
        else:
            #print(m["minute"],date.strftime("%Y-%m-%d"));
            pass;
        key=generated_date.strftime("%Y-%m-%d");
        if not key in csv:
            csv[key]=dict();
        csv[key][m["minute"]]=m["count"];

def process(hex,t):
    t1=datetime.datetime.strptime(t,"%Y-%m-%d %H:%M:%S.%f");
    t0=datetime.datetime.strptime("2019-01-19 18:00","%Y-%m-%d %H:%M");
    if not t1>t0:
        return;
    merge(t1,hamster.statistics.asJson(hex));

def getdb(filename):
    global sqlite;
    global conn;
    create=not os.path.exists(filename);
    conn = sqlite3.connect(filename);
    sqlite = conn.cursor();
    
def main():
    global sqlite;
    global conn;
    global csv;
    
    filename="sqlite.db";
    getdb(filename);
    
    cur = conn.cursor()
    cur.execute("SELECT req,time FROM requests")
    rows = cur.fetchall()
    for row in rows:
        (req,t)=row;
        if isinstance(req,bytes):
            process(req.hex(),t);
    for date in csv:
        filename=date+".csv";
        f=open(filename,'w');
        d = "";
        for m in sorted(csv[date]):
            #print(date, m);
            d += str(m)+","+str(csv[date][m])+"\n";
        #print(d);
        f.write(d);

if __name__ == "__main__":
    main();

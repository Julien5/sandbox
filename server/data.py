#!/usr/bin/env python3

import sys;
import os;
import sqlite3;

import datetime;
import json;
import subprocess;
import binascii;
import math;

def try_create(sqlite,stm):
    try:
        sqlite.execute(stm);
    except sqlite3.OperationalError as e:
        pass; # print("could not run:",stm);

class request:
    def __init__(self,ID,path,data,time):
        self.ID=ID;
        self.path=path;
        self.data=data;
        self.time=time;

    def __str__(self):
        return "%05d %40s %03d bytes %s" % (self.ID,self.path,len(self.data),self.time.strftime("%Y-%m-%d %H:%M:%S"));
      
class Sql:
    def __init__(self):
        create = not os.path.exists("sqlite.db");
        self.conn = sqlite3.connect("sqlite.db");
        self.sqlite = self.conn.cursor();
        try_create(self.sqlite,"CREATE TABLE requests (ID INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT, data BLOB, time TEXT)");
        self.conn.commit();
        
    def insert_request(self,path,data):
        t=str(datetime.datetime.now());
        self.sqlite.execute('INSERT INTO requests (path,data,time) VALUES (?,?,?)', (path, data, t));
        if data and "tickscounter" in path:
            self.insert_ticks(ticks_from_post_data(data,t)); 
        self.conn.commit();

    def read_request(self,path):
        ret=[];
        self.sqlite.execute("SELECT * FROM requests WHERE path LIKE (?)",("%"+path+"%",))
        rows = self.sqlite.fetchall()
        for row in rows:
            r_id=int(row[0]);
            r_path=row[1];
            r_data=row[2];
            r_time=datetime.datetime.strptime(row[3], '%Y-%m-%d %H:%M:%S.%f');
            ret.append(request(r_id,r_path,r_data,r_time));
        return ret;    

def main():
    sql=Sql();
    R=sql.read_request("/compteur/tickcounter/data");
    print(len(R));
    for r in R:
        print(str(r))
        
if __name__ == "__main__":
    main();

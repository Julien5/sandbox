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
        
if __name__ == "__main__":
    pass;

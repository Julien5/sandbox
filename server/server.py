#!/usr/bin/env python3
 
from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse;
import sqlite3;
import os;
import datetime;
import cgi;
import binascii;
import data;
import html;
import sys;
import webdir;

def log(msg):
    print("log:",msg);
    f=open("server.log",'a');
    f.write(msg);
    f.write("\n");
    f.close();

def read_file(path):
    return open(path, 'rb').read();

dataprocessor = data.Data();
database = data.Sql();

class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def setup(self):
        BaseHTTPRequestHandler.setup(self)
        self.request.settimeout(30)
              
    def do_GET(self):
        log("GET request for {}".format(self.path));
        global dataprocessor,database;
        database.insert(self.path,bytes());
        
        self.send_response(200)
        # Send message back to client
        message=None;
        if self.path.endswith("html"):
            url=self.path;
            filename=urllib.parse.unquote(url);
            filename = os.path.join("html",filename[1:]);
            message = read_file(filename).decode("ascii");
        elif "png" in self.path:
            url=self.path;
            filename=urllib.parse.unquote(url);
            filename = filename[1:];
            statinfo = os.stat(filename)
            img_size = statinfo.st_size
            self.send_header("Content-length", img_size);
            message = read_file(filename);
        elif "update" in self.path:
            data.dump(database,dataprocessor);
            message = "updated";
        elif self.path == "/time":
            message = "{%s}" % (datetime.datetime.now());
        elif self.path == "/message":
            sms=dataprocessor.sms();
            message = "{"+sms+"}";
        elif self.path == "/sunw": # seconds_until_next_wifi
            T1=datetime.datetime.now();
            T2=T1+datetime.timedelta(minutes=5);
            # T2=T1.replace(day=T1.day+1,hour=10,minute=0,second=0)
            message = "{%d}" % int((T2-T1).total_seconds());
        else:
            message = "unknown command."
            
        if isinstance(message,str):
            message += '\n';
            message = bytes(message, "utf8");
            self.send_header('Content-type',"text/html");
        else:
            self.send_header('Content-type',"image/png");
            
        assert(isinstance(message,bytes));
        self.end_headers()
        self.wfile.write(message)
        log("good.");

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length) 
        log("POST request for {} at {}".format(self.path,datetime.datetime.now()));
        log("received {} bytes".format(len(post_data)));
        database.insert(self.path,post_data);
        data.update(database,dataprocessor);
        sms=dataprocessor.sms();
        log("sms:"+sms);
        message = sms;
        # Write content as utf-8 data
        self.wfile.write(bytes(message, "utf8"));        
        log("good.");
    
    
def run():
    log('starting server...')
    data.update(database,dataprocessor);
    # Server settings
    server_address = ('0.0.0.0', 8000)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    log('running server...')
    while True:
        try:
            httpd.serve_forever()
        except Exception as e:
            log("exception:",str(e));            

def main():
    run();
    
if __name__ == "__main__":
    main();

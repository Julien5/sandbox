#!/usr/bin/env python3
 
from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse;
import sqlite3;
import os;
import datetime;
import time;
import cgi;
import binascii;
import data;
import html;
import sys;
import webdir;
import mail;

def log(msg):
    print("log:",msg);
    f=open("server.log",'a');
    f.write(msg);
    f.write("\n");
    f.close();

def read_file(path):
    return open(path, 'rb').read();

database = data.Sql();

class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def setup(self):
        BaseHTTPRequestHandler.setup(self)
        self.request.settimeout(30)
              
    def do_GET(self):
        log("GET request for {}".format(self.path));
        global database;
        database.insert_request(self.path,bytes());
        ticksHandler=data.TicksHandler(database.select_ticks());
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
            data.dump(database,ticksHandler);
            message = "updated";
        elif self.path == "/time":
            message = "{%s}" % (datetime.datetime.now());
        elif self.path == "/utime":
            message = "{%s}" % int(time.time());
        elif self.path == "/message":
            sms=ticksHandler.sms();
            message = "{"+sms+"}";
        elif self.path == "/string":
            message = ticksHandler.string().replace("\n","</p>");
        elif self.path == "/stats":
            message = ticksHandler.stats().replace("\n","</p>");
        elif self.path == "/sunw": # seconds_until_next_wifi
            T1=datetime.datetime.now();
            # T2=T1+datetime.timedelta(minutes=5);
            H=7;
            T2=T1.replace(hour=H,minute=0,second=0)
            d=T1.day;
            if T1>T2:
                T2=T2.replace(day=T2.day+1);
            assert(T2>T1);
            message = "{%d}" % int((T2-T1).total_seconds());
        else:
            message = read_file("html/index.html").decode("ascii");
            
        if isinstance(message,str):
            message += '\n';
            message = bytes(message, "utf8");
            self.send_header('Content-type',"text/html");
        else:
            self.send_header('Content-type',"image/png");
            
        assert(isinstance(message,bytes));
        self.end_headers()
        while message:
            n=self.wfile.write(message);
            message=message[n:];
        log("good.");

    def do_POST(self):
        global database;
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length) 
        log("POST request for {} at {}".format(self.path,datetime.datetime.now()));
        log("received {} bytes".format(len(post_data)));
        database.insert_request(self.path,post_data);
        sms=data.TicksHandler(database.select_ticks()).sms();
        log("sms:"+sms);

        if "tickscounter" in self.path:
            msg="path={0}\nsize:{1}\nsms:{2}".format(self.path,len(post_data),sms);
            mail.sendmail("message de google",msg);

        message = "{"+sms+"}";
        self.wfile.write(bytes(message, "utf8"));        
        log("good.");
    
    
def run():
    log('starting server...')
    # Server settings
    server_address = ('0.0.0.0', 8000)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    # data.rebuild();
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

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
import sys;
import mail;

import get_compteur;

def log(msg):
    print("log:",msg);
    f=open("server.log",'a');
    f.write(msg);
    f.write("\n");
    f.close();

def read_file(path):
    return open(path, 'rb').read();

database = data.Sql();

def htmlize(s):
    tmpl = '''
 <!DOCTYPE html>
<html>
<body>
<p>{string}</p>
</body>
</html>
    '''
    s=s.replace("\n","</p>\n<p>");
    ret=tmpl.replace("{string}",s);
    return ret;

class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def setup(self):
        BaseHTTPRequestHandler.protocol_version = 'HTTP/1.0'
        BaseHTTPRequestHandler.setup(self)
        self.request.settimeout(30)
              
    def do_GET(self):
        log("GET request for {}".format(self.path));
        global database;
        database.insert_request(self.path,bytes());
        header=True;
        message="hello\n";
        if self.path == "/epoch":
            header=False;
            message=str(int(time.time()));
        if self.path == "/compteur/tickscounter":
            message=htmlize(get_compteur.get_tickscounter(database,10));
            log(str(len(message)));
        if self.path.endswith(".html") or self.path.endswith(".csv"):
            filename=self.path;
            if filename[0] == "/":
                filename=filename[1:];
            if os.path.exists(filename):
                message=open(filename,"r").read();
            else:
                print("could not find",filename);
        message = bytes(message, "utf8");
        if header:
            self.send_response(200);
            self.send_header('Content-Type',"text/html");
            self.end_headers()
        assert(isinstance(message,bytes));
        log(str(len(message)));
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
        log("hex:"+post_data.hex());
        database.insert_request(self.path,post_data);
        message = "{good}\n";
        self.wfile.write(bytes(message, "utf8"));        
        log("good.");
        
def run():
    log('starting server...')
    # Server settings
    server_address = ('0.0.0.0', 8000)
    testHTTPServer_RequestHandler.protocol = "HTTP/1.0";
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

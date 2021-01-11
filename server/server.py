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
        message="hello";
        message += '\n';
        message = bytes(message, "utf8");
        self.send_header('Content-Type',"text/html");
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
        log("hex:"+post_data.hex());
        database.insert_request(self.path,post_data);
        message = "{good}\n";
        self.wfile.write(bytes(message, "utf8"));        
        log("good.");
        
def run():
    log('starting server...')
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

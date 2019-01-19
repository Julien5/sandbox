#!/usr/bin/env python3
 
from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse;
import sqlite3;
import os;
import datetime;
import cgi;
import binascii;

conn=None;
sqlite=None;

# HTTPRequestHandler class
class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def setup(self):
        BaseHTTPRequestHandler.setup(self)
        self.request.settimeout(30)
    
    def do_GET(self):
        global sqlite;
        global conn;
        t=str(datetime.datetime.now());
        sqlite.execute('INSERT INTO requests (req,time) VALUES (?,?)', (self.path, t));
        conn.commit();
        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()
        # Send message back to client
        if "time" in self.path:
            message = "clock: "+t+" [end]"; 
        else:
            message = "thanks,bye"
        message += "\n";
        # Write content as utf-8 data
        self.wfile.write(bytes(message, "utf8"))
        return

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length) 
        print("POST request for {}".format(self.path).encode('utf-8'));
        t=str(datetime.datetime.now());
        data=post_data; #.decode('utf-8');
        sqlite.execute('INSERT INTO requests (req,time) VALUES (?,?)', (data, t));
        conn.commit();
        print("received {} bytes".format(len(data)));
        # print(binascii.hexlify(data).decode('UTF-8'));
        message = "thanks,bye\n"
        # Write content as utf-8 data
        self.wfile.write(bytes(message, "utf8"));
        print("good.");
    
    
def run():
    print('starting server...')
    # Server settings
    server_address = ('0.0.0.0', 8000)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    print('running server...')
    while True:
        try:
            httpd.serve_forever()
        except Exception as e:
            print("exception:",str(e));            

def getdb(filename):
    global sqlite;
    global conn;
    create=not os.path.exists(filename);
    conn = sqlite3.connect(filename);
    sqlite = conn.cursor();
    if create:
        sqlite.execute("CREATE TABLE requests (ID INT PRIMARY KEY, req TEXT, time REAL)");
                   
def main():
    filename="sqlite.db";
    getdb(filename);
    run();
    
if __name__ == "__main__":
    main();

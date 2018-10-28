#!/usr/bin/env python3
 
from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse;
import sqlite3;
import os;
import datetime;

conn=None;
sqlite=None;

# HTTPRequestHandler class
class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
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
        message = "thanks,bye"
        # Write content as utf-8 data
        self.wfile.write(bytes(message, "utf8"))
        return
    
def run():
    print('starting server...')
    # Server settings
    server_address = ('0.0.0.0', 8080)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    print('running server...')
    httpd.serve_forever()

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

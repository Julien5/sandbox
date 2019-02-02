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

def log(msg):
    print("log:",msg);
    f=open("server.log",'a');
    f.write(msg);
    f.write("\n");
    f.close();

def read_image(path):
    return open(path, 'rb').read();

def dir_list(path):
    r = []
    try:
        displaypath = urllib.parse.unquote(path,
                                           errors='surrogatepass')
    except UnicodeDecodeError:
        displaypath = urllib.parse.unquote(path)
    displaypath = html.escape(displaypath, quote=False)
    enc = sys.getfilesystemencoding()
    title = 'Directory listing for %s' % displaypath
    r.append('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" '
             '"http://www.w3.org/TR/html4/strict.dtd">')
    r.append('<html>\n<head>')
    r.append('<meta http-equiv="Content-Type" '
             'content="text/html; charset=%s">' % enc)
    r.append('<title>%s</title>\n</head>' % title)
    r.append('<body>\n<h1>%s</h1>' % title)
    r.append('<hr>\n<ul>')
    for name in reversed(sorted(os.listdir(path))):
        fullname = os.path.join(path, name)
        if not ".png" in name:
            continue;
        displayname = linkname = name
        # Append / for directories or @ for symbolic links
        if os.path.isdir(fullname):
            displayname = name + "/"
            linkname = name + "/"
        if os.path.islink(fullname):
            displayname = name + "@"
            # Note: a link to a directory displays with @ and links with /
        r.append('<li><a href="%s">%s</a></li>'
                 % (urllib.parse.quote(linkname,
                                       errors='surrogatepass'),
                    html.escape(displayname, quote=False)))
        r.append('</ul>\n<hr>\n</body>\n</html>\n')
    return '\n'.join(r);

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
        if "time" in self.path:
            t=str(datetime.datetime.now());
            message = "clock: "+t+" [end]";
        elif "millis/list.html" in self.path:
            message = dir_list("millis");
        elif "minutes/list.html" in self.path:
            message = dir_list("minutes");
        elif "png" in self.path:
            url=self.path;
            filename=urllib.parse.unquote(url);
            # remove front /
            print(url);
            print(filename);
            filename = filename[1:];
            statinfo = os.stat(filename)
            img_size = statinfo.st_size
            self.send_header("Content-length", img_size);
            message = read_image(filename);
        elif "update" in self.path:
            data.dump(database,dataprocessor);
            message = "updated";
        elif "message" in self.path:
            sms="T:100K L24=12345XXXXXXXXXx";
            sms=dataprocessor.sms();
            #    1234567890123456
            message = "{"+sms+"}";
        else:
            message = "thanks,bye"
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
        log("POST request for {}".format(self.path));
        log("received {} bytes".format(len(post_data)));
        database.insert(self.path,post_data);
        data.update(database,dataprocessor);
        log("sms:"+dataprocessor.sms());
        message = "thanks,bye\n"
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

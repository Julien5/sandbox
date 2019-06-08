#!/usr/bin/env python3

import ann;
import datetime;
import os;
import shutil;

def main():
    if not os.path.exists("old"):
        os.makedirs("old");
    if os.path.exists("results"):
        aname=datetime.datetime.today().strftime("%Y%m%d.%H%M%S");
        shutil.move("results","old/"+aname);

    for nhidden in range(5,40,2):
        ann.learn(nhidden);
        
if __name__ == '__main__':
    main();

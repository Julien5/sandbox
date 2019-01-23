#!/usr/bin/env python3

import statistics
#import bytearray

def main():
    hx = str(open("testdata.txt").read().strip());
    print(statistics.statistics.asJson(hx));
    
if __name__ == "__main__":
    print("__main__");
    main();

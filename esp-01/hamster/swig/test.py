#!/usr/bin/env python3

import hamster
import binascii

def main():
    hx = str(open("testdata.txt").read().strip());
    print(hamster.statistics.asJson(hx));
    
    x=open("../tests/tickscounter.bin",'rb').read()
    hx = binascii.hexlify(x).decode("ascii");
    print(hamster.tickscounter.asJson(hx));
    
if __name__ == "__main__":
    print("__main__");
    main();

#!/usr/bin/env python3

import hamster

def main():
    hx = str(open("testdata.txt").read().strip());
    print(hamster.statistics.asJson(hx));
    C=hamster.tickscounter();
    
if __name__ == "__main__":
    print("__main__");
    main();

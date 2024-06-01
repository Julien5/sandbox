#!/usr/bin/env python3

class List:
    def __init__(self):
        self.D = dict();

    def append(self,filename,size):
        if not size in self.D:
            self.D[size]=list();
        self.D[size].append(filename);
        
    def stats(self):
        R=dict();
        # 1995840
        for size in self.D:
            Lsize=self.D[size];
            if not len(Lsize) in R:
                R[len(Lsize)] = 0;
            R[len(Lsize)] += 1;
        for n in sorted(R.keys()):
            print(f"number of {n:d}-duplicates: {R[n]:d}");

        
def main():
    filename="/home/julien/tmp/tmp/lists/all.extended";
    f=open(filename,'r');
    lines=f.read().split("\n");
    L=List();
    print("reading..");
    for line in lines:
        if not "|" in line:
            continue;
        parts=line.split("|");
        size=int(parts[1]);
        filename=parts[3];
        L.append(filename,size);
    L.stats();
    return 1

if __name__ == "__main__":
   main();

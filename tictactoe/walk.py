#!/usr/bin/env python3

class Board:
    # 1,2,3
    # 8,9,4
    # 7,6,5
    def __init__(self, X, O):
        self.X = set(X);
        self.O = set(O);

    def children(self):
        N=3;#9;
        if len(self.X) == len(self.O):
            # X's turn.
            for i in range(N):
                p = i+1;
                if p in self.X or p in self.O:
                    continue;
                else:
                    X=set(self.X);
                    X.add(p);
                    yield Board(X,self.O);
        else:
            # O's turn
            for i in range(N):
                p = i+1;
                if p in self.X or p in self.O:
                    continue;
                else:
                    O=set(self.O);
                    O.add(p);
                    yield Board(self.X,O);

    def string(self):
        return "X:"+str(self.X)+"O:"+str(self.O);


def walk(b):
    print(b.string());
    for c in b.children():
        walk(c);
        
def main():
    b=Board(set(),set());
    walk(b);

if __name__ == '__main__':
    main()

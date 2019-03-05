#!/usr/bin/env python3

N=9;

class Board:
    # 0,1,2
    # 7,8,3
    # 6,5,4
    def __init__(self, B=' '*N):
        self.board = B;

    def _winner(self,I):
        L=[self.board[i] for i in I];
        l0 = L[0]
        if l0 == ' ':
            return None;
        for l in L:
            if l != l0:
                return None;
        return l0;

    def winner(self):
        b=self.board;
        II=list();
        II.append({0,8,4});
        II.append({0,7,6});
        II.append({1,8,5});
        II.append({2,3,4});
        II.append({0,1,2});
        II.append({7,8,3});
        II.append({6,5,4});
        II.append({6,8,2});
        for I in II:
            w=self._winner(I);
            if w:
                return w;
        return None;
            
    def stop(self):
        w=self.winner();
        return self.winner() != None;

    def free(self):
        if self.stop():
            return [];
        return [i for i in range(len(self.board)) if self.board[i]==' '];
    
    def child(self,position):
        assert(position in self.free());
        assert(self.board[position] == ' ');
        xturn=self.board.count('x') == self.board.count('o');
        sign='o';
        if xturn:
            sign='x';
        L=list(self.board);
        L[position]=sign;
        b=''.join(L);
        return Board(b);
    
    def children(self):
        for p in self.free():
            yield self.child(p);
            
    def string(self):
        return self.board;


def walk(b):
    print(b.string());
    for c in b.children():
        walk(c);
        
def main():
    b=Board();
    walk(b);

if __name__ == '__main__':
    main()

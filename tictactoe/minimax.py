#!/usr/bin/env python3

import os;
import pickle;
import random;

N=9;
class Board:
    # 0,1,2
    # 7,8,3
    # 6,5,4
    def prettyprint(self):
        b=self.board;
        s=[];
        s.append("{}|{}|{}".format(b[0],b[1],b[2]));
        s.append("{}|{}|{}".format(b[7],b[8],b[3]));
        s.append("{}|{}|{}".format(b[6],b[5],b[4]));
        return "\n".join(s);
    
    def __init__(self, B=' '*N):
        self.board = B;

    def normalize(self):
        return Board(min(Board.siblings(self.board)));

    def __hash__(self):
        return hash(self.board);

    def __eq__(self,other):
        return self.board == other.board;
        
    def __str__(self):
        return "["+self.board+"]";
     
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

    def xturn(self):
        return self.board.count('x') == self.board.count('o');
    
    def getscore(self):
        w=self.winner();
        if w == 'x':
            return 1;
        if w == 'o':
            return 0;
        if not self.free(): # draw
            return 0.5;
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
        sign='o';
        if self.xturn():
            sign='x';
        L=list(self.board);
        L[position]=sign;
        b=''.join(L);
        return Board(b);
    
    def children(self):
        F=self.free();
        random.shuffle(F);
        for p in F:
            yield self.child(p);

def minimax(b):
    if b.xturn():
        return max;
    return min;
            
def argminmax(S,minmax):
    assert(S);
    m=minmax(S.values());
    for c in S:
        if S[c] == m:
            return c;
    assert(0);
    return None;
            
def score(b):
    s=b.getscore();
    if not s is None:
        return s;
    S = dict();
    for c in b.children():
        S[c] = score(c);
        # alpha,beta-pruning
        if S[c] == int(b.xturn()): 
            return S[c];
    return minimax(b)(S.values());

def computerplay(b):
    S = dict();
    for c in b.children():
        S[c] = score(c);
        # alpha,beta-pruning
        if S[c] == int(b.xturn()):
            return c;
    return argminmax(S,minimax(b));

def humanplay(b):
    F=b.free();
    print("choices:",F);
    pos=-1;
    while not pos in F:
        pos=int(input('you:'));
    return b.child(pos);

def finished(b):
    F=b.free();
    if not F:
        print("winner:",b.winner());
        return True;
    return False;

def nextplay(b,computerplayx):
    if computerplayx == b.xturn():
        b=computerplay(b);
        print("computer:");
        print(b.prettyprint());
    else:
        b=humanplay(b);
        print(b.prettyprint());
    return b;    
        
def play():
    b=Board();
    #assert(computerplay(Board('o      xx')).board == 'o  o   xx');
    # 'x' always starts
    computerplayx=random.randint(0,1) == 0;
    while not finished(b):
        b=nextplay(b,computerplayx);
      
if __name__ == '__main__':
    play();

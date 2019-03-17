#!/usr/bin/env python3

import os;
import pickle;
import random;
import math;

N=9;
class Board:
    # 0,1,2
    # 7,8,3
    # 6,5,4
    def transform(I,b):
        L = list(b);
        R = [];
        for i in I:
            R.append(L[i]);
        return ''.join(R);
        
    def rotations():
        T=[];
        T.append([0,1,2,3,4,5,6,7,8]);
        T.append([2,3,4,5,6,7,0,1,8]);
        T.append([4,5,6,7,0,1,2,3,8]);
        T.append([6,7,0,1,2,3,4,5,8]);
        return T;

    def flips():
        T=[];
        T.append([0,1,2,3,4,5,6,7,8]);
        T.append([2,1,0,7,6,5,4,3,8]);
        T.append([6,5,4,3,2,1,0,7,8]);
        return T;

    def siblings(B):
        ret=set();
        for r in Board.rotations():
            for f in Board.flips():
                ret.add(Board.transform(f,Board.transform(r,B)));
        return ret;

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

            
scoretxt = "score.pickle";
true_score = pickle.load(open(scoretxt,'rb'));
score_table=dict();
def Qscore(_b):
    # 1 iff 'x' wins
    # 0 iff 'o' wins
    # 0.5 otherwise
    global score_table;
    b=_b.normalize();
    if not b in score_table:
        score_table[b]=0.5;
    
    s=b.getscore();
    if not s is None:
        score_table[b]=s;

    return score_table[b];

def Qlearn(_b,target):
    global score_table;
    b=_b.normalize();
    assert(b in score_table);
    assert(0<=target and target<=1);
    alpha=0.6;
    score_table[b] = alpha*score_table[b] + (1-alpha)*target;

def argmax(S,m):
    assert(S);
    for c in S:
        if S[c] == m:
            return c;
    assert(0);
    return None;

def computerplay(b):
    S=dict();        
    for c in b.children():
        S[c]=Qscore(c);
        if random.randint(0,20) == 0:
            return c;
    m = min(S.values());
    if b.xturn():
        m = max(S.values());
    return argmax(S,m);

def finished(b):
    F=b.free();
    if not F:
        #print("winner:",b.winner());
        return True;
    return False;

def nextplay(b):
    return computerplay(b);

nplay=0;
def play():
    global score_table,nplay;
    
    b=Board();
    # 'x' always starts
    path=list();
    while not finished(b):
        b=nextplay(b);
        path.append(b);
        #print(b.prettyprint());
        
    xwins=b.winner() and not b.xturn();
    owins=b.winner() and b.xturn();
    draw=(not xwins) and (not owins);
    target=None;
    if owins:
        target=0;
    if xwins:
        target=1;
    if draw:
        target=0.5;
    
    for p in path:
        Qlearn(p,target);
           
    e=0;
    for b in score_table:
        #print(b.board,score_table[b],score[b]);
        e+=abs(score_table[b.normalize()]-true_score[b]);

    nplay = nplay + 1;
    print(nplay,len(score_table),e/len(score_table),end="\r");

      
if __name__ == '__main__':
    while True:
        play();
        #exit(0);

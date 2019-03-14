#!/usr/bin/env python3

import os;
import pickle;
import random;

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
        for p in self.free():
            yield self.child(p);

visits=0;
def walk_children(b,tree=None):
    if tree is None:
        tree=dict();
        
    global visits;
    visits = visits + 1;
    if visits % 1000 == 0:
        percent=100*len(tree)/765;
        print('progress:{percent:{width}.1f}% visits:{visits:{widthv}}'.format(percent=percent, width=3, visits=visits, widthv=6),end="\r");
    if not b in tree:
        tree[b.normalize()]=set(); 
    for c in b.children():
        tree[b.normalize()].add(c.normalize());
        walk_children(c,tree);
    return tree;

def walk_score(b,tree,score=None):
    if score is None:
        score=dict();
    assert(b.normalize()==b);
    if b in score:
        return score;
     
    s=b.getscore();
    if not s is None:
        score[b]=s;
        return score;
 
    C=tree[b];
    assert(C);
    S = [walk_score(c,tree,score)[c] for c in C];
    if b.xturn():
        score[b]=max(S);
    else:
        score[b]=min(S);
    return score;

treetxt = "tree.txt";
scoretxt = "score.txt";

def build():
    if not os.path.exists(treetxt):
        print("compute tree");
        tree=walk_children(Board());
        pickle.dump(tree,open(treetxt,'wb'));

    print("load tree");
    tree = pickle.load(open(treetxt,'rb'));
    print("loaded tree");
    print("number of nodes:",len(tree));
   
    if not os.path.exists(scoretxt):
        print("compute scores");
        score=walk_score(Board(),tree);
        pickle.dump(score,open(scoretxt,'wb'));
             
    score = pickle.load(open(scoretxt,'rb'));
    print("loaded score");
    print("number of scores:",len(score));

def getposition(b,cn):
    assert(cn.normalize() == cn);
    for position in b.free():
        if b.child(position).normalize() == cn:
            return position;
    return None;    

def computerplay(b,tree,score):
    bn = b.normalize();
    C = tree[bn];
    S = [score[c] for c in C];
    m = min(S);
    if b.xturn():
        m = max(S);
    position=None;
    for cn in C:
        if score[cn] == m:
            return b.child(getposition(b,cn));
    return None;

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

def nextplay(b,tree,score,computerplayx):
    if computerplayx == b.xturn():
        b=computerplay(b,tree,score);
        print("computer:");
        print(b.prettyprint());
    else:
        b=humanplay(b);
        print(b.prettyprint());
    return b;    
        
def play():
    tree = pickle.load(open(treetxt,'rb'));
    score = pickle.load(open(scoretxt,'rb'));
    b=Board();
    # 'x' always starts
    computerplayx=random.randint(0,1) == 0;
    while not finished(b):
        b=nextplay(b,tree,score,computerplayx);
      
if __name__ == '__main__':
    build();
    play();

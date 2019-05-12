#!/usr/bin/env python3

import board;
import random;
import pickle;
import layer;
import dataset;
import numpy as np;
import glob;
import os;

def argmax(S,m):
    assert(S);
    for c in S:
        if S[c] == m:
            return c;
    assert(0);
    return None;

scoretxt = "score.pickle";
true_score = pickle.load(open(scoretxt,'rb'));
def Tscore(_b):
    global true_score;
    b=_b.normalize();
    return true_score[b];

def layerfile():
    list_of_files = glob.glob('layer.*.pickle') # * means all if need specific format then *.csv
    return max(list_of_files, key=os.path.getctime);

def layers():
    return pickle.load(open(layerfile(),'rb'));

def ann_play(b):
    L=layers();
    X=np.zeros((len(b.board),1));
    X[:,0]=dataset.board_to_vector(b);
    Y=layer.propagate(X,L);
    R=range(Y.shape[0]);
    S=[Y[i,0] for i in R];
    for i in R:
        if not i in b.free():
            S[i]=0;
    for i in R:
        if S[i] == max(S):
            return b.child(i);
    assert(0);
    return b;

def computerplay(b,strategy=0):
    # strategy = 0 => random 
    # strategy = 1 => minimax
    # strategy = 2 => neural net
    assert(b.free());
    if strategy == 0:
        F=b.free();
        random.shuffle(F)
        return b.child(F[0]);

    if strategy == 2:
        return ann_play(b);
        
    S=dict();        
    for c in b.children():        
        if strategy == 1:
            S[c]=Tscore(c);
        
    m = min(S.values());
    if b.xturn():
        m = max(S.values());
    return argmax(S,m);

def winner(xstrategy,ostrategy):
    b=board.Board();
    while not board.finished(b):
        strategy = xstrategy;
        if not b.xturn():
            strategy = ostrategy;
        b=computerplay(b, strategy);
    if b.winner() == 'x':
        return xstrategy;
    else:
        return ostrategy;
    # draw
    return None;

def perf_against_random():
    W=[];
    for i in range(100):
        W.append(winner(2,0));
    return 100*W.count(2)/len(W);

def perf_against_minimax():
    W=[];
    for i in range(100):
        W.append(winner(2,1));
    return 100*W.count(None)/len(W);

def main():
    print("use:",layerfile());
    print("random:",perf_against_random(),"%");
    print("minimax:",perf_against_minimax(),"%");
    
if __name__ == '__main__':
    assert(winner(1,0)==1);
    assert(winner(0,1)==1);
    main();

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
    C=[];
    for c in S:
        if S[c] == m:
            C.append(c);
    assert(C);
    random.shuffle(C);
    return C[0];

import gen;
true_score=None;
def Tscore(_b):
    global true_score;
    if not true_score:
        tree,true_score=gen.build();
    b=_b.normalize();
    return true_score[b];

def layerfiles():
    return sorted(glob.glob('res/layer.*.pickle'),key=os.path.getctime);

_layerfile=None;
def layerfile():
    if _layerfile:
        return _layerfile;
    list_of_files = layerfiles();
    if not list_of_files:
        return None;
    return max(list_of_files, key=os.path.getctime);

def layers():
    if not layerfile():
        return None;
    return pickle.load(open(layerfile(),'rb'));

def play_ann(b):
    F=b.free();
    if not F:
        return None;
    
    L=layers();
    assert(L);
    
    best=None;
    for c in b.children():
        X=np.zeros((len(b.board),1));
        X[:,0]=dataset.board_to_vector(c.normalize());
        Y=layer.propagate(X,L);
        assert(Y.shape == (1,1));
        score = Y[0];
        if not best or best[0]<score:
            best=(score,c);
    assert(best);
    return best[1];

def play_minimax(b):
    F=b.free();
    if not F:
        return None;
    
    S=dict();        
    for c in b.children():        
        S[c]=Tscore(c);
        
    m = min(S.values());
    if b.xturn():
        m = max(S.values());
    return argmax(S,m);

def play_random(b):
    F=b.free();
    if not F:
        return None;
    random.shuffle(F) 
    return b.child(F[0]);

def display_error():
    b=board.Board();
    L=layers();
    X=np.zeros((len(b.board),1));
    while b:
        for c in b.children():
            X[:,0]=dataset.board_to_vector(c.normalize());
            Y=layer.propagate(X,L);
            print("output:",Y[0]," target:",Tscore(c))
        print("--");
        if not b.free():
            print("winner:",b.winner());
            break;
        if b.xturn():
            b=play_ann(b);
        else:
            b=play_minimax(b);
    
        
def computerplay(b,strategy=0):
    # strategy = 0 => random 
    # strategy = 1 => minimax
    # strategy = 2 => neural net
    assert(b.free());
    if strategy == 0:
        return play_random(b);

    if strategy == 1:
        return play_minimax(b);
    
    if strategy == 2:
        return play_ann(b);

    print("error: strategy invalid");
    assert(0);
    return None;

def winner(xstrategy,ostrategy):
    b=board.Board();
    while not board.finished(b):
        strategy = xstrategy;
        if not b.xturn():
            strategy = ostrategy;
        b=computerplay(b, strategy);
    if b.winner() == 'x':
        return xstrategy;
    elif b.winner() == 'o':
        return ostrategy;
    # draw
    return None;

def perf_against_random():
    W=[];
    for i in range(10):
        W.append(winner(2,0));
    return 100*W.count(2)/len(W);

def perf_against_minimax():
    W=[];
    for i in range(10):
        W.append(winner(2,1));
    return 100*W.count(None)/len(W);

def main():
    print("use:",layerfile()," info:",open(layerfile().replace(".pickle",".info"),'r').read().rstrip());
    print("random:",perf_against_random(),"% minimax:",perf_against_minimax(),"%");
    
if __name__ == '__main__':
    assert(winner(1,0) in {None,1});
    assert(winner(0,1) in {None,1});
    for f in layerfiles():
        _layerfile=f;
        main();
    #display_error();

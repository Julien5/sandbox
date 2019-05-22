#!/usr/bin/env python3

import board;
import random;
import pickle;
import layer;
import dataset;
import numpy as np;
import glob;
import os;
import sys;

def argmax(S,m):
    assert(S);
    C=[];
    for c in S:
        if S[c] == m:
            C.append(c);
    assert(C);
    print([(c.board,S[c]) for c in S]);
    print([(c.board,S[c]) for c in C]);
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
    global _layerfile;
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
    assert(not b.xturn());
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

def play_human(b):
    print(b.prettyprint());
    choice=None;
    while not choice or choice not in b.free():
        choice=int(input("choice:"+str(b.free())));
    return b.child(choice);

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
    # strategy = 3 => human
    assert(b.free());
    if strategy == 0:
        return play_random(b);

    if strategy == 1:
        return play_minimax(b);
    
    if strategy == 2:
        return play_ann(b);

    if strategy == 3:
        return play_human(b);

    print("error: strategy invalid");
    assert(0);
    return None;

def winner(xstrategy,ostrategy):
    b=board.Board();
    B=[];
    while not board.finished(b):
        strategy = xstrategy;
        if not b.xturn():
            strategy = ostrategy;
        b=computerplay(b, strategy);
        B.append(b);
    if True and ostrategy==3 or xstrategy==3:
        print("finished");
        print(b.prettyprint());
    if True and xstrategy==2 and ostrategy==1 and b.winner()!='o':
        for x in B:
            print(x.board);
        print(b.prettyprint());
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
    return 100*(W.count(2)+W.count(None))/len(W);

def perf_against_minimax():
    W=[];
    for i in range(10):
        W.append(winner(2,1));
    return 100*W.count(None)/len(W);

def process():
    #print("use:",layerfile()," info:",open(layerfile().replace(".pickle",".info"),'r').read().rstrip());
    N,X,Target=dataset.get();
    print(layerfile(),":","random:", perf_against_random(),"% minimax:",perf_against_minimax(),"%","Jall=",layer.J(X,Target,layers()));
    #if perf_against_minimax()==100:
    #    winner(2,3);
    
def main():
    global _layerfile;
    #assert(winner(1,0) in {None,1});
    #assert(winner(0,1) in {None,1});
    _layerfile=None;
    if len(sys.argv) >= 2:
        _layerfile=sys.argv[1];
    if _layerfile and not os.path.isfile(_layerfile):
        _layerfile=None;
    process();
    #display_error();

if __name__ == '__main__':
    main();

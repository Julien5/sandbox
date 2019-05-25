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

def Arg(S,m):
    assert(S);
    C=[];
    for c in S:
        if S[c] == m:
            C.append(c);
    #print([(c.board,S[c]) for c in S]);
    #print([(c.board,S[c]) for c in C]);
    if not C:
        return None;
    return C;

def argmax(S,m):
    assert(S);
    C=Arg(S,m);
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

def dict_ann(L,b):
    S=dict();
    for c in b.children():
        X=np.zeros((len(b.board),1));
        X[:,0]=dataset.board_to_vector(c.normalize());
        Y=layer.propagate(X,L);
        assert(Y.shape == (1,1));
        S[c]=Y[0][0];
    return S;

def dict_minimax(b):
    S=dict();        
    for c in b.children():        
        S[c]=Tscore(c.normalize());
    return S;

def play_ann(b):
    F=b.free();
    if not F:
        return None;
    
    L=layers();
    assert(L);
    S=dict_ann(L,b);
    m=max(S.values());
    return argmax(S,m);

def minimax_equivalent(L,b):
    S1=dict_ann(L,b);
    S2=dict_minimax(b);
    
    if (not S1) and (not S2):
        return True;
    if (not S1) != (not S2):
        return False;

    # not all max must coincide
    # all ann-max must be a minimax-max.
    A1=Arg(S1,max(S1.values()));
    A2=Arg(S2,max(S2.values()));
    ret=set(A1).issubset(set(A2));
    return ret;

def diff_minimax(layers):
    tree,_score=gen.build();
    b=board.Board('x   x xoo')
    #assert(b.normalize() in tree);
    #print(b.prettyprint());
    #print(dict_minimax(b).values());
    #print(dict_ann(layers,b).values());
    good=0;
    total=0;
    E=[];
    D=[];
    for b in tree:
        if not b.xturn():
            continue;
        if minimax_equivalent(layers,b):
            E.append(b);
        else:
            D.append(b);
    return E,D;
    
def play_minimax(b):
    F=b.free();
    if not F:
        return None;
    
    S=dict_minimax(b); 
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

def winner(init,xstrategy,ostrategy):
    b=init;
    B=[];
    while not board.finished(b):
        strategy = xstrategy;
        if not b.xturn():
            strategy = ostrategy;
        b=computerplay(b, strategy);
        B.append(b);
    if ostrategy==3 or xstrategy==3:
        print("finished");
        print(b.prettyprint());
    # ann play against minimax.
    if False and xstrategy==2 and ostrategy==1 and b.winner() != 'x':
        d=any([x.xturn() and not minimax_equivalent(layers(),x) for x in B]);
        if d:
            print("---- start ---"); 
            for x in B:
                if not d:
                    continue;
                print(x.prettyprint());
                print("");
                if x.xturn() and not minimax_equivalent(layers(),x):
                    print(dict_ann(layers(),x).values());
                    print(dict_minimax(x).values());
            print("---- end -----"); 
    return b.winner();

def perf_against_random(D):
    W=[];
    for d in D:
        W.append(winner(d,2,0));
    return 100*(W.count('x')+W.count(None))/len(W);

def perf():
    # starting from this board
    tree,_score=gen.build();
    E=[];
    D=[];
    for d in tree:
        if not d.xturn():
            continue;
        w2=winner(d,2,1);
        w1=winner(d,1,1);
        if w2==w1:
            E.append(d);
        else:
            D.append(d);
    return E,D;

def perf_against_minimax(D):
    W=[];
    for d in D:
        W.append(winner(d,2,1));
    return 100*W.count(None)/len(W);

def percent(E,D):
    return 100*len(E)/(len(E)+len(D));

def process():
    #print("use:",layerfile()," info:",open(layerfile().replace(".pickle",".info"),'r').read().rstrip());
    N,X,Target=dataset.get();
    Em,Dm=diff_minimax(layers());
    Ep,Dp=perf();
    print(layerfile(),len(Ep)+len(Dp),": minimax:",percent(Ep,Dp),"%","Jall=",layer.J(X,Target,layers())," ME:",percent(Em,Dm),"%");
    

def present():    
    np.set_printoptions(precision=2)
    np.set_printoptions(suppress=True)
    print(layerfile());
    L=layers();
    print("number of layers:",len(L));
    for i in range(len(L)):
        W=L[i].W;
        print(i,"#input :",W.shape[1]-1, "=> #output:",W.shape[0]);
        print(W);       
    
def main():
    global _layerfile;
    #assert(winner(1,0) in {None,1});
    #assert(winner(0,1) in {None,1});
    filename=None;
    if len(sys.argv) >= 2:
        filename=sys.argv[1];
    if filename and not os.path.isfile(filename):
        filename=None;
    L=[];
    if not filename:
        L=layerfiles();
    else:
        L=[filename];
        _layerfile=filename;
        present();
    for l in L:
        _layerfile=l;
        process();
    #display_error();

if __name__ == '__main__':
    main();

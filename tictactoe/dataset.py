#!/usr/bin/env python3

import gen
import numpy as np;
import random;

def board_to_vector(B):
    N=len(B.board);
    x=np.zeros(N);
    for i in range(N):
        b=B.board[i];
        if b=='x':
            x[i]=1;
        elif b=='o':
            x[i]=-1;
        elif b==' ':
            x[i]=0;
        else:
            assert(0);
    return x;

def board_to_target(score,B):
    N=len(B.board);
    y=np.zeros(1);
    y[0]=score[B.normalize()];
    return y;

def get(part=100):
    tree,score=gen.build();
    N=[];
    for b in tree:
        N.append(len(b.board));
        N.append(board_to_target(score,b).shape[0]);
        break;
    
    T=int(len(tree)*part/100);
    assert(T<=len(tree));
    assert(N[0]==9);
    X=np.zeros((N[0],T));
    Target=np.zeros((N[1],T));
    t=0;
    B=list(tree.keys());
    random.shuffle(B);
    B=B[0:T];
    for b in B:
        X[:,t]=board_to_vector(b);        
        Target[:,t]=board_to_target(score,b);
        t=t+1;
    return X,Target;

def mini(key):
    if key == "xor":
        N=[2,2,1];
        T=4;
        function=lambda x: int(x[0]!=x[1]);
    elif key == "sin":
        N=[1,200,50,1];
        T=20;
        function=lambda x: math.sin(x[0]);
        
    X=np.zeros((N[0],T));
    Target=np.zeros((N[-1],T)); 
    for t in range(T):
        x = np.zeros(N[0]);
        if key == 'xor':
            x[0] = int(t&2>0);
            x[1] = int(t&1>0);
        if key == 'sin':
            x[0] = random.random();
        X[:,t]=x;        
        Target[0,t]=function(X[:,t]);
    return X,Target;

def main():
    X,Target=get(25);
    for t in range(X.shape[1]):
        print("X:",X[:,t]);
        print("T:",Target[:,t]);
    
if __name__ == '__main__':
    main();

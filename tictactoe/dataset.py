#!/usr/bin/env python3

import gen
import numpy as np;

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

def board_to_target_0(score,B):
    N=len(B.board);
    y=np.zeros(N);
    if not B.free():
        return y;
    # y[i] = 1 if i is a position that maximizes the score
    #        0 otherwise
    M=max([score[c.normalize()] for c in B.children()]);
    for i in B.free():
        c=B.child(i).normalize();
        assert(c in score);
        if score[c]==M:
            y[i]=1;
    return y;        

def board_to_target_1(score,B):
    N=len(B.board);
    y=np.zeros(1);
    y[0]=score[B.normalize()];
    return y;

def board_to_target(score,B):
    return board_to_target_1(score,B);

def get():
    tree,score=gen.build();
    N=[];
    Ntarget=0;
    for b in tree:
        N.append(len(b.board));
        N.append(board_to_target(score,b).shape[0]);
        break;
    T=len(tree);
    assert(N[0]==9);
    X=np.zeros((N[0],T));
    Target=np.zeros((N[1],T));
    t=0;
    for b in tree:
        X[:,t]=board_to_vector(b);        
        Target[:,t]=board_to_target(score,b);
        t=t+1;
    # hidden layers;
    N.insert(1,27);
    # N.insert(1,27);
    return N,X,Target;

def main():
    N,X,Target=get();
    print(N);
    for t in range(X.shape[1]):
        print("X:",X[:,t]);
        print("T:",Target[:,t]);
    
if __name__ == '__main__':
    main();

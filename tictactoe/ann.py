#!/usr/bin/env python3

import math;
import random;
import numpy as np;
import dataset;
import layer;

def propagate(X,layers):
    K = len(layers);
    for k in range(K):
        if k == 0:
            layers[k].propagate(X);
        else:
            layers[k].propagate(layers[k-1].Y);

def learn(X,T,layers):
    K = len(layers);
    propagate(X,layers);       
    # backprop
    nG=[];
    for k in reversed(range(K)):
        if k == K-1:
            layers[k].settarget(T);
        else:
            layers[k].backpropagate(layers[k+1]);
        nGk=layers[k].adapt();
        nG.append(nGk);
    if not any(nG):
        return False;
    return True;

def J(X,Target,layers):
    propagate(X,layers);
    Y=layers[-1].Y;
    T=X.shape[1];
    return sum([layer.C(Y[:,t],Target[:,t]) for t in range(T)]);

def minidataset(key):
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
    return N,X,Target;

def decreasing(s):
    if len(s)<10:
        return all(x>y for x, y in zip(s, s[1:]));
    last = s[-1];
    Last = s[-10:];
    mean = sum(Last)/len(Last);
    return last < mean;

def main():
    #N,X,Target = minidataset('sin');
    N,X,Target = dataset.get();

    T=X.shape[1];
     # init
    layers=[];
    for i in range(1,len(N)):
        layers.append(layer.Layer(N[i-1],N[i],layer.activation('sigma'),0.001));    
    iter=0;
    scores=[];
    while not scores or decreasing(scores):
        scores.append(J(X,Target,layers));
        if iter % 10 == 0:
            print("J=",scores[-1])
        learn(X,Target,layers);
        iter = iter + 1;
    print("J=",scores[-1]);

if __name__ == '__main__':
    main();

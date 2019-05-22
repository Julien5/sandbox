#!/usr/bin/env python3

import math;
import random;
import numpy as np;
import dataset;
import layer;
import pickle;

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
    N,X,Target = dataset.get(50);

    T=X.shape[1];
     # init
    layers=[];
    for i in range(1,len(N)):
        layers.append(layer.Layer(N[i-1],N[i],layer.activation('sigma'),0.001));    
    iter=0;
    scores=[];
    while not scores or decreasing(scores):
        scores.append(layer.J(X,Target,layers));
        if iter % 50 == 0:
            print("J=",scores[-1])
            filename="res/layer."+str(iter);
            pickle.dump(layers,open(filename+".pickle",'wb'));
            open(filename+".info",'w').write("J="+str(scores[-1])+"\n");
        layer.learn(X,Target,layers);
        iter = iter + 1;
    print("J=",scores[-1]);

if __name__ == '__main__':
    main();

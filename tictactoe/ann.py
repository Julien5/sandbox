#!/usr/bin/env python3

import math;
import os;
import random;
import numpy as np;
import dataset;
import layer;
import pickle;

def decreasing(s):
    if len(s)<10:
        return all(x>y for x, y in zip(s, s[1:]));
    last = s[-1];
    Last = s[-10:];
    mean = sum(Last)/len(Last);
    return last < mean;

def learn(nhidden):
    X,Target = dataset.get(50);
    N=[];
    # input layer
    N.append(X.shape[0]);
    
    # hidden layer(s)
    N.append(nhidden);

    # output layer
    N.append(Target.shape[0]);
    
    T=X.shape[1];
    # init
    layers=[];
    for i in range(1,len(N)):
        layers.append(layer.Layer(N[i-1],N[i],layer.activation('sigma'),0.001));
        
    dirname="{}/{}".format("results",nhidden);
    if not os.path.exists(dirname):
        os.makedirs(dirname);

    scores=[];
    iter=0;
    while not scores or (decreasing(scores) and iter<200):
        scores.append(layer.J(X,Target,layers));
        if iter % 50 == 0:
            pfile="{}/{}.pickle".format(dirname,iter);
            pickle.dump(layers,open(pfile,'wb'));
            ifile="{}/{}.info".format(dirname,iter);
            open(ifile+".info",'w').write("J="+str(scores[-1])+"\n");
            print("J=",scores[-1],pfile,ifile)
        layer.learn(X,Target,layers);
        iter = iter + 1;
    print("J=",scores[-1]);
    return scores[-1];

def main():
    learn(3);
    
if __name__ == '__main__':
    main();

#!/usr/bin/env python3

import math;
import numpy as np;

def f(L):
    L[0]=0;

def main():
    N = 3;
    M = 2;

    D = {};
    D[f] = 1;

    d=np.ones((1,));
    #d=np.reshape(d,(1,1));
    X=np.ones((3,));
    X=np.reshape(X,(3,1));
    print(d);
    print(X);
    print(X.T);
    np.dot(d,np.transpose(X));
    
    L=[1,2,3];
    print(L);
    f(L);
    print(L);
    print(L[-1]);

    x = np.zeros((M+1,));
    for i in range(M+1):
        x[i]=i;
    print("x",x);
    x = np.delete(x,x.shape[0]-1,0)
    print("x",x);
    
    x = np.zeros((M,));
    


    
    x[0]=1;
    x[1]=3;
    x = np.append(x, [1], axis = 0);
    print(x);



    W = np.zeros((N,M+1));
    for i in range(min(N,M+1)):
        W[i,i]=1;
    print(W);
    z=np.dot(W,x);
    print(z);


if __name__ == '__main__':
    main();

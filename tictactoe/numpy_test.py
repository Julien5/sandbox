#!/usr/bin/env python3

import math;
import numpy as np;

def f(L):
    L[0]=0;

def main():
    N = 3;
    M = 2;
    
    L=[1,2,3];
    print(L);
    f(L);
    print(L);
    print(L[-1]);
    
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

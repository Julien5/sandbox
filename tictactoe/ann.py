#!/usr/bin/env python3

import math;
import numpy as np;

def _f(x):
    c=10;
    return 0.5*(1+math.tanh(c*x));

npf = None;
def f(x):
    global npf;
    if not npf:
        npf=np.vectorize(_f);
    return npf(x);

def _df(x):
    c=10;
    return c*0.5*(1-math.pow(math.tanh(c*x),2));

npdf = None;
def df(x):
    global npdf;
    if not npdf:
        npdf=np.vectorize(_f);
    return npdf(x);

def mult(A,B):
    return np.dot(A,B);

def norm(A):
    return np.linalg.norm(A,2);

def d(x,t,A,b):
    return f(mult(A,x)+b)-t;

def fp(x,t,A,b):
    return df(mult(A,x)+b);

def dp(x,t,A,b):
    d_ = d(x,t,A,b);
    fp_ = fp(x,t,A,b);
    return mult(np.diag(d_),fp_);

def dJ(X,T,A,b):
    I = range(len(X));
    dA = sum([mult(dp(X[i],T[i],A,b),np.transpose(X[i])) for i in I]);
    db = sum([dp(X[i],T[i],A,b) for i in I]);
    return [dA,db];

def J(X,T,A,b):
    I = range(len(X));
    return sum([np.linalg.norm(f(mult(A,X[i])+b)-T[i],2) for i in I]);

def distance(A,Aold):
    return norm(A-Aold);
    
def learn(X,T):
    N=T[0].shape[0];
    M=X[0].shape[0];
    A=np.zeros((N,M));
    A[0,0]=1;
    A[0,1]=0;
    b=np.zeros(N);
    mu=0.5;
    while J(X,T,A,b)>0: 
        d = dJ(X,T,A,b);
        d = d/norm(d);
        A = A - mu*d[0];
        b = b - mu*d[1];
        print(J(X,T,A,b));
    print("A:",A," b:",b);
    return A;

def main():
    X=[];
    T=[]; 
    for i in range(4):
        x = np.zeros((2,1));
        x[0] = int(i&2>0);
        x[1] = int(i&1>0);
        X.append(x);
        T.append(np.array([int(x[0] or x[1])]));
    learn(X,T);

if __name__ == '__main__':
    main();

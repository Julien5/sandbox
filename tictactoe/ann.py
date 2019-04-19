#!/usr/bin/env python3

import math;
import numpy as np;

def _f(x):
    c=1;
    return 0.5*(1+math.tanh(c*x));

npf = None;
def f(x):
    global npf;
    if not npf:
        npf=np.vectorize(_f);
    return npf(x);

def _df(x):
    c=1;
    return c*0.5*(1-math.pow(math.tanh(c*x),2));

npdf = None;
def df(x):
    global npdf;
    if not npdf:
        npdf=np.vectorize(_f);
    return npdf(x);

def d(x,t,A,b):
    return f(A*x+b)-t;

def fp(x,t,A,b):
    return df(A*x+b);

def dp(x,t,A,b):
    d_ = d(x,t,A,b);
    fp_ = fp(x,t,A,b);
    return np.diag(d_)*fp_;

def dJ(X,T,A,b):
    I = range(len(X));
    dA = sum([X[i]*np.transpose(dp(X[i],T[i],A,b)) for i in I]);
    i=0;
    print(X[i].shape)
    print(dp(X[i],T[i],A,b).shape);
    print(dA.shape);
    print("--");
    db = sum([dp(X[i],T[i],A,b) for i in I]);
    return [dA,db];

def J(X,T,A,b):
    I = range(len(X));
    return sum([np.linalg.norm(f(A*X[i]+b)-T[i],2) for i in I]);

def distance(A,Aold):
    return np.linalg.norm(A-Aold,2);

def learn(X,T):
    A=np.zeros((T[0].shape[0],X[0].shape[0]));
    A[0,0]=1;
    b=np.zeros(X[0].shape[0]);
    print(A.shape,b.shape);
    mu=1;
    epsilon=mu/10;
    Aold = None;
    while not Aold or distance(A,Aold)>epsilon:
        d = dJ(X,T,A,b);
        A = A - mu*d[0];
        b = b - mu*d[1];
        #A[0] = A[0]/math.fabs(A[0]);
        print(A.shape,b.shape);
        return;
        #print(J(X,T,A,b));
    return A;

def main():
    X=[];
    T=[]; 
    for i in range(4):
        x1=int(i&2>0);
        x2=int(i&1>0);
        X.append(np.array([x1,x2]));
        T.append(np.array([int(x1 or x2)]));
    learn(X,T);

if __name__ == '__main__':
    main();

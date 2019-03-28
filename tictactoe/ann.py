#!/usr/bin/env python3

import math;

def f(x):
    return 0.5*(1+math.tanh(x));

def df(x):
    return 0.5*(1-math.pow(math.tanh(x),2));

def dJ(X,T,A):
    a = A[0];
    b = A[1];
    da = sum([X[i]*df(a*X[i]+b)*(f(a*X[i]+b)-T[i]) for i in range(len(X))]);
    db = sum([     df(a*X[i]+b)*(f(a*X[i]+b)-T[i]) for i in range(len(X))]);
    #print(da,db);
    return [da,db];

def distance(A,Aold):
    return sum([math.pow(A[i]-Aold[i],2) for i in range(len(A))]);

def learn(X,T):
    A=[1,1];
    mu=0.5;
    epsilon=mu/100;
    Aold = None;
    while not Aold or distance(A,Aold)>epsilon:
        d = dJ(X,T,A);
        A = [A[i] - mu*d[i] for i in range(len(A))];
        A[0] = A[0]/math.fabs(A[0]);
        print(A);
    return A;

def main():
    X=[0,1,2,3,4,5,6,7,8,9];
    T=[0,0,0,0,0,1,1,1,1,1];
    A=learn(X,T);
    print(A);

if __name__ == '__main__':
    main();

#!/usr/bin/env python3

import math;
import numpy as np;

def _sigma(x):
    c=10;
    return 0.5*(1+math.tanh(c*x));
npf = None;
def sigma(x):
    global npf;
    if not npf:
        npf=np.vectorize(_sigma);
    return npf(x);

def _dsigma(x):
    c=10;
    return c*0.5*(1-math.pow(math.tanh(c*x),2));
            
npdf = None;
def dsigma(x):
    global npdf;
    if not npdf:
        npdf=np.vectorize(_dsigma);
    return npdf(x);

def norm(x):
    return np.linalg.norm(x,2);

class Layer:
    # M inputs, N outputs
    
    def __init__(self,M,N):
        self.W = np.zeros((N,M+1));
        for i in range(min(N,M+1)):
            self.W[i,i]=1;
        pass;

    def propagate(self,x):
        M = self.W.shape[1]-1;
        assert( x.shape == (M,1) );
        self.x = np.append(x, [[1]], axis = 0);
        assert( self.x.shape == (M+1,1) );
        self.z=np.dot(self.W,self.x);
        self.y=sigma(self.z);

    def backpropagate(self,layer):
        self.dC=np.dot(np.transpose(layer.W), np.multiply(dsigma(layer.z),layer.dC));

    def settarget(self,t):
        self.dC=2*(self.y - t);
        
    def adapt(self):
        dW=np.dot(np.multiply(dsigma(self.z),self.dC),np.transpose(self.x));
        mu = 0.05;
        assert(self.W.shape == dW.shape);
        if norm(dW)>0:
            self.W = self.W - mu*dW/norm(dW);
            return True;
        return False;

def propagate(x,layers):
    K = len(layers);
    for k in range(K):
        if k == 0:
            layers[k].propagate(x);
        else:
            layers[k].propagate(layers[k-1].y);

def learn(x,t,layers):
    prev=None;
    K = len(layers);
    propagate(x,layers);       
    # backprop
    for k in reversed(range(K)):
        if k == K-1:
            layers[k].settarget(t);
        else:
            layers[k].backpropagate(layer[k+1]);
        layers[k].adapt();

def _J(x,t,layers):
    propagate(x,layers);
    y=layers[-1].y;
    return norm(y-t);

def J(X,T,layers):
    return sum([_J(X[i],T[i],layers) for i in range(len(X))]);
        
def main():
    N=[2,1];
    # init
    layers=[];
    for i in range(1,len(N)):
        layers.append(Layer(N[i-1],N[i]));

    X=[];
    T=[]; 
    for i in range(4):
        x = np.zeros((2,1));
        x[0] = int(i&2>0);
        x[1] = int(i&1>0);
        X.append(x);
        T.append(np.array([int(x[0] and x[1])]));
       
    while True:
        for i in range(len(X)):
            learn(X[i],T[i],layers);
        print(J(X,T,layers),layers[-1].W);
       
    


if __name__ == '__main__':
    main();

#!/usr/bin/env python3

import math;
import random;
import numpy as np;

def sigma(x):
    c=1;
    return 0.5*(1+math.tanh(c*x));

def dsigma(x):
    c=1;
    return c*0.5*(1-math.pow(math.tanh(c*x),2));

def activation(name):
    if name == 'sigma':
        return [sigma,dsigma];
    return None;

def norm(x):
    return np.linalg.norm(x,2);

def C(y,t):
    d=(t-y);
    return float(d.T*d);

def dC(y,t):
    d=y-t;
    return 2*d;
       
class Layer:
    # M inputs, N outputs    
    def __init__(self,M,N,A,mu):
        assert(len(A)==2);
        self.activation = [np.vectorize(a) for a in A];
        self.mu = mu;
        self.W = np.zeros((N,M+1));
        self.W = np.random.randn(N,M+1)*np.sqrt(1/M);
        for i in range(min(N,M+1)):
            self.W[i,M]=0;
        pass;

    def sigma(self,x):        
        return self.activation[0](x);

    def dsigma(self,x):
        return self.activation[1](x);

    def propagate(self,X):
        M = self.W.shape[1]-1;
        assert( X.shape[0] == M );
        T = X.shape[1];
        self.X = np.append(X, np.ones((1,T)), axis = 0);
        assert( self.X.shape == (M+1,T) );
        self.Z=np.dot(self.W,self.X);
        self.Y=self.sigma(self.Z);

    def backpropagate(self,layer):
        # remove bias
        T = self.X.shape[1];
        W = np.delete(layer.W,layer.W.shape[1]-1,1);
        assert(layer.dC.shape[1]==T);
        D=np.dot(np.transpose(W), layer.dC);
        assert(D.shape[1]==T);
        self.dC=np.multiply(self.dsigma(self.Z),D);
        assert(self.dC.shape == self.Z.shape);
        
    def settarget(self,Target):
        T = Target.shape[1];
        self.dC=np.multiply(dC(self.Y,Target),self.dsigma(self.Z));
        assert(self.dC.shape[1]==T);
            
    def adapt(self):
        # mean
        T = self.X.shape[1];
        N = self.W.shape[0];
        M = self.X.shape[0]-1;
        dW = np.zeros((N,M+1));
        for t in range(T):
            # self.X[:,t] is a 1D array with length M+1
            # => np.transpose does nothing ("there is nothing to transpose");
            # note: use X.T
            dCt = np.reshape(self.dC[:,t],(N,1));
            Xt = np.reshape(self.X[:,t],(M+1,1));
            dW=dW+np.dot(dCt,np.transpose(Xt));
        assert(self.W.shape == dW.shape);
        if norm(dW)>0:
            self.W = self.W - self.mu*dW/norm(dW);
            return True;
        else:
            print("warning: null gradient");
        return False;

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
    return sum([C(Y[:,t],Target[:,t]) for t in range(T)]);

def dataset(key):
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
    N,X,Target = dataset('sin');

    T=X.shape[1];
     # init
    layers=[];
    for i in range(1,len(N)):
        layers.append(Layer(N[i-1],N[i],activation('sigma'),0.001));    
    iter=0;
    scores=[];
    while not scores or decreasing(scores):
        scores.append(J(X,Target,layers));
        if iter % 100 == 0:
            print("J=",scores[-1])
        learn(X,Target,layers);
        iter = iter + 1;
    print("J=",scores[-1]);

if __name__ == '__main__':
    main();

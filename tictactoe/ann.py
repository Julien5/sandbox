#!/usr/bin/env python3

import math;
import time;
import numpy as np;

def _sigma(x):
    c=100;
    return 1/(1+math.exp(-c*x));
    return 0.5*(1+math.tanh(c*x));

npf = None;
def sigma(x):
    global npf;
    if not npf:
        npf=np.vectorize(_sigma);
    return npf(x);

def _dsigma(x):
    c=100;
    return _sigma(x)*(1-_sigma(x));
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
            self.W[i,i]=0*1;
            self.W[i,M]=0*-0.5;
        pass;

    def propagate(self,X):
        M = self.W.shape[1]-1;
        assert( X.shape[0] == M );
        T = X.shape[1];
        self.X = np.append(X, np.ones((1,T)), axis = 0);
        assert( self.X.shape == (M+1,T) );
        self.Z=np.dot(self.W,self.X);
        self.Y=sigma(self.Z);

    def backpropagate(self,layer):
        # remove bias
        T = self.X.shape[1];
        W = np.delete(layer.W,layer.W.shape[1]-1,1);
        assert(layer.dC.shape[1]==T);
        D=np.dot(np.transpose(W), layer.dC);
        assert(D.shape[1]==T);
        self.dC=np.multiply(dsigma(self.Z),D);
        assert(self.dC.shape == self.Z.shape);
        
    def settarget(self,Target):
        T = Target.shape[1];
        self.dC=2*np.multiply((self.Y - Target),dsigma(self.Z));
        assert(self.dC.shape[1]==T);
            
    def adapt(self):
        # mean
        T = self.X.shape[1];
        N = self.W.shape[0];
        M = self.X.shape[0]-1;
        dW = np.zeros((N,M+1));
        for t in range(T):
            dCt = np.reshape(self.dC[:,t],(N,1));
            Xt = np.reshape(self.X[:,t],(M+1,1));
            dW=dW+np.dot(dCt,np.transpose(Xt));
        mu = .01/T;
        assert(self.W.shape == dW.shape);
        if norm(dW)>0:
            self.W = self.W - mu*dW/norm(dW);
            return True;
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
        # print("layer:",k);
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
    return sum([norm(Y[:,t]-Target[:,t]) for t in range(T)]);
        
def main():
    N=[2,2,2,2,1];
    # init
    layers=[];
    for i in range(1,len(N)):
        layers.append(Layer(N[i-1],N[i]));

    T=60;
    X=np.zeros((2,T));
    Target=np.zeros((1,T)); 
    for t in range(T):
        x = np.zeros(2);
        x[0] = int(t&2>0);
        x[1] = int(t&1>0);
        X[:,t]=x;
        Target[0,t]=int(x[0] or x[1]);

    iter=0;
    while J(X,Target,layers)>0:
        print("J=",J(X,Target,layers))
        learn(X,Target,layers);
        iter = iter + 1;
        #time.sleep(0.1);
    


if __name__ == '__main__':
    main();

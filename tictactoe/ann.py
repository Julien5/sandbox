#!/usr/bin/env python3

import math;
import numpy as np;

def _sigma(x):
    c=1;
    return 0.5*(1+math.tanh(c*x));
npf = None;
def sigma(x):
    global npf;
    if not npf:
        npf=np.vectorize(_sigma);
    return npf(x);

def _dsigma(x):
    c=1;
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

    def propagate(self,x):
        M = self.W.shape[1]-1;
        assert( x.shape == (M,1) );
        self.x = np.append(x, [[1]], axis = 0);
        assert( self.x.shape == (M+1,1) );
        self.z=np.dot(self.W,self.x);
        self.y=sigma(self.z);

    def backpropagate(self,layer):
        #self.dC=np.dot(np.transpose(layer.W), np.multiply(dsigma(layer.z),layer.dC));
        d=np.dot(np.transpose(layer.W), layer.dC);
        d=np.delete(d,d.shape[0]-1,0)
        self.dC=np.multiply(dsigma(self.z),d);
        
        #print("   l.dC=",norm(layer.dC));
        #print("l.ds(z)=",norm(dsigma(layer.z)));
        #self.dC = np.delete(self.dC,self.dC.shape[0]-1,0)
        assert(self.dC.shape == self.y.shape);
        
    def settarget(self,t):
        self.dC=2*(self.y - t);
        
    def adapt(self):
        dW=np.dot(np.multiply(dsigma(self.z),self.dC),np.transpose(self.x));
        #print("dC=",norm(self.dC));
        #print("dW=",norm(dW));
        mu = 0.1;
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
    K = len(layers);
    propagate(x,layers);       
    # backprop
    debug=0;
    #print("input:",np.transpose(x));
    for k in reversed(range(K)):
        #print("layer:",k);
        if k == K-1:
            layers[k].settarget(t);
        else:
             layers[k].backpropagate(layers[k+1]);
        layers[k].adapt();



def _J(x,t,layers):
    propagate(x,layers);
    y=layers[-1].y;
    return norm(y-t);

def J(X,T,layers):
    return sum([_J(X[i],T[i],layers) for i in range(len(X))]);
        
def main():
    N=[2,2,2,1];
    # init
    layers=[];
    for i in range(1,len(N)):
        layers.append(Layer(N[i-1],N[i]));
        print(layers[-1].W);
  
    X=[];
    T=[]; 
    for i in range(4):
        x = np.zeros((2,1));
        x[0] = int(i&2>0);
        x[1] = int(i&1>0);
        X.append(x);
        T.append(np.array([int(x[0] or x[1])]));

    iter=0;
    while J(X,T,layers)>0:
        for i in range(len(X)):
            learn(X[i],T[i],layers);
        print("---- iter -------");
        for l in layers:
            print("W=",l.W);            
        print("J=",J(X,T,layers));
        if iter==100:
            break;
        iter = iter + 1;
    


if __name__ == '__main__':
    main();

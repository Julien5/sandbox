#!/usr/bin/env python3

import math;

def f(x):
    return math.cos(x*x);

def df(x):
    return -2*x*math.sin(x*x);

def gradient(f):
    x = 0.1;
    xold = None;
    mu=0.10;
    epsilon = mu/1000.0;
    niter=0;
    while (not xold) or math.fabs(x-xold)>epsilon:
        xold = x;
        x = x - mu*df(x);
        niter = niter + 1;
    print("niter:",niter);
    return (x,f(x));

def main():
    (x,m)=gradient(f);
    print("fmin={} at x={}".format(m,x));

if __name__ == '__main__':
    main();

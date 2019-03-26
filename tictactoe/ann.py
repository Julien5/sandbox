#!/usr/bin/env python3

def f(x):
    return x*x;

def gradient(f):
    return (0,f(0));

def main():
    (x,m)=gradient(f);
    print("fmin={} at x={}".format(m,x));

if __name__ == '__main__':
    main();

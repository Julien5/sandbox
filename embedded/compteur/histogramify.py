#!/usr/bin/env python3

import sys;

def clean(s):
	S=s.split();
	ret=list();
	for s in S:
		if s:
			try:
				ret.append(int(s));
			except:
				pass;
	return ret;

def norm(L):
	M=max(L);
	if M==0:
		M=1	
	for k in range(len(L)):
		L[k]=L[k]/M
	return L;

def main(filename):
	f=open(filename,'r');
	M=list();
	N=0;
	for line in f.readlines():
		L=clean(line);
		if not L:
			continue;
		L=norm(L);
		if not N:
			N=len(L)	
		if len(L)==N:
			M.append(L);

	for k in range(N):
		for m in range(len(M)):
			print(M[m][k],end=" ");
		print();
		
if __name__ == "__main__":
    main(sys.argv[1])

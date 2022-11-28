#!/usr/bin/env python3

import sys
import copy;

def neighbours(a):
	(n,m)=a;
	R=set();
	for k in [-1,0,+1]:
		for l in [-1,0,+1]:
			R.add((n+k,m+l));
	return R;

def segments(indexset):
	R=list();
	A=copy.deepcopy(indexset);
	C=set();
	while A:
		Q=None;
		for a in A:
			Q=[a];
			break;
		if not Q:
			return;
		assert(Q);
		while Q:
			a=Q.pop(0);
			for n in A.intersection(neighbours(a)):
				if not n in C:
					C.add(n); # label
					Q.append(n); # add to the queue
		R.append(copy.deepcopy(C));
		A=A-C;
		C.clear();
	RB=list();	
	return R;

if __name__ == '__main__':
	sys.exit(main())  

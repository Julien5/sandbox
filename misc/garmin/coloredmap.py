#!/usr/bin/env python3

import sys

def colors(B):
	ret=dict();
	for k in range(len(B)):
		for index in B[k]:
			if not index in ret:
				ret[index]=set();
			ret[index].add(k);
	return ret;

def indexes(C):
	ret=dict();
	for index in C:
		color=C[index];
		colort=tuple(color);
		if not colort in ret:
			ret[colort]=set();
		ret[colort].add(index);
	return ret;

if __name__ == '__main__':
	sys.exit(main())  

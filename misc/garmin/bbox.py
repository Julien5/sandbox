#!/usr/bin/env python3

import sys

def cell(c):
	nmin=min([n for (n,m) in c.area()]);
	nmax=max([n for (n,m) in c.area()])+1;
	mmin=min([m for (n,m) in c.area()]);
	mmax=max([m for (n,m) in c.area()])+1;
	return (nmin,nmax,mmin,mmax);

def union(bbox1,bbox2):
	(nmin1,nmax1,mmin1,mmax1)=bbox1;
	(nmin2,nmax2,mmin2,mmax2)=bbox2;
	nmins=[nmin1,nmin2];
	mmins=[mmin1,mmin2];
	nmaxs=[nmax1,nmax2];
	mmaxs=[mmax1,mmax2];
	return (min(nmins),max(nmaxs),min(mmins),max(mmaxs));

def intersection(bbox1,bbox2):
	(nmin1,nmax1,mmin1,mmax1)=bbox1;
	(nmin2,nmax2,mmin2,mmax2)=bbox2;
	nmins=[nmin1,nmin2];
	mmins=[mmin1,mmin2];
	nmaxs=[nmax1,nmax2];
	mmaxs=[mmax1,mmax2];
	return (max(nmins),min(nmaxs),max(mmins),min(mmaxs));

def cells(C):
	ret=None;
	for c in C:
		if ret is None:
			ret=cell(c);
		else:
			ret=union(ret,cell(c));
	return ret;


if __name__ == '__main__':
	sys.exit(main())  


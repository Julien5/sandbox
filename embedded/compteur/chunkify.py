#!/usr/bin/env python3

import sys;
a=0;
alpha=0.0;
A=list();
while True:
	x=sys.stdin.read(1);
	if not x:
		break;
	if not x.isalnum():
		continue;
	if len(A)<4:
		A.append(x);
	if len(A)<4:
		continue;
	# u16: 2bytes, little endian
	hex=A[2:4]+A[0:2];
	x=int("0x"+"".join(hex),0);
	a=alpha*a + (1-alpha)*x;
	print(x)
	A=list();


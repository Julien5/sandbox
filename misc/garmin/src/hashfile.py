#!/usr/bin/env python3

import sys
import hashlib

# BUF_SIZE is totally arbitrary, change for your app!
BUF_SIZE = 65536;  # lets read stuff in 64kb chunks!

def get(filename):
	md5 = hashlib.md5()
	with open(filename, 'rb') as f:
		while True:
			data = f.read(BUF_SIZE)
			if not data:
				break
			md5.update(data)
	return md5.hexdigest();		

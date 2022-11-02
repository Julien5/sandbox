#!/usr/bin/env python3

import readgpx;
import sys;

def main(filename):
	tracks=readgpx.tracks(filename);
	print(len(tracks));
	for t in tracks:
		print(t.string());
	
if __name__ == '__main__':
	if (len(sys.argv)) < 2:
		print("error");
		exit(1);
	filename=sys.argv[1];
	sys.exit(main(filename))  

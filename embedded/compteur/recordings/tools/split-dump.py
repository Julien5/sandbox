#!/usr/bin/env python3

import fileinput
import sys

# [1198130]-> value:515 xalpha:519 delta:004 threshold:072
for line in fileinput.input():
	s=line.rstrip();
	time=int(s.split("]")[0][1:])/1000;
	parts=s.split(" ");
	for part in parts:
		if not ":" in part:
			continue;
		pair=part.split(":");
		marker=pair[0];
		value=float(pair[1]);
		# print(pair[0],time,pair[1]);
		# sys.stdout.write("%s %06d %04d" %		 )
		print(f'{marker:12s}:{time:04.2f} {value:03.2f}')


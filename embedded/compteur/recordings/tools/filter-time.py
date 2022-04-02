#!/usr/bin/env python3

import sys
start=int(sys.argv[1]);
end=-1;
if len(sys.argv)>2:
		end=int(sys.argv[2]);
# [1198130]-> value:515 xalpha:519 delta:004 threshold:072
for line in sys.stdin:
	s=line.rstrip();
	time=float(s.split(" ")[0]);
	if end<0 and start <= time:
		print(s);
		continue;
	if start <= time and time <= end:
		print(s);
		continue;
	


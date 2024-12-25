#!/usr/bin/env python3

import re;

def read(filename):
	f=open(filename,'r');
	C=f.read();
	ret=dict();
	for _line in C.split("\n"):
		line=_line.strip();
		if not line:
			continue;
		if line.startswith("#"):
			continue;
		parts=line.split("=");
		key=parts[0];
		value="=".join(parts[1:]);
		if key in ret:
			print(line);
		ret[key]=value;
	return ret;

def resolve(P,s):
	K=re.findall(r'\{\S+\}', s);
	if not K:
		return s;
	ret=s;
	R=[k[1:-1] for k in K if k[1:-1] in P];
	if not R:
		return ret;
	for key in R:
		assert(key in P)
		ret=ret.replace("{"+key+"}",P[key]);
		print(key,"->",P[key]);
	return resolve(P,ret);
	
def main():
	#K=re.findall(r'^\S+=', "aa.ss={foo.bar}/bar= dd");
	#print(K);
	#K=re.findall(r'=\S+', "aa.ss={foo.bar}/bar = d");
	#print(K);
	#return;
	coredir="/opt/esp8266-toolchain/Arduino-3.1.2";
	platform=f"{coredir:s}/platform.txt"
	boards=f"{coredir:s}/boards.txt"
	P=read(platform);
	#for k in ["compiler.path","compiler.c.flags","compiler.cpp.flags"]:
	for k in ["recipe.cpp.o.pattern"]:
		v=resolve(P,P[k]);
		print(f"{k:s}={v:s}");
	#rint(P["compiler.path"]);

if __name__ == "__main__":
	main();

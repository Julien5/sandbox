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
	K=re.findall(r'\{\S+\}', s.replace("}{","} {"));
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
	K=re.findall(r'{\S+}', "aa.ss={foo.bar}d/ {ff.dd}/bar= dd");
	print(K);
	#return
	#K=re.findall(r'=\S+', "aa.ss={foo.bar}/bar = d");
	#print(K);
	#return;
	coredir="/opt/esp8266-toolchain/Arduino-3.1.2";
	platform=f"{coredir:s}/platform.txt"
	boards=f"{coredir:s}/boards.txt"
	P=read(platform);
	B=read(boards);
	for b in B:
		if b.startswith("nodemcu."):
			unb=".".join(b.split(".")[1:])
			if unb in P:
				if unb == "name":
					continue;
				print("clash at",unb);
				print("P:",P[unb]);
				print("B:",B[b]);
				assert(False)
			P[unb]=B[b];
			#print("from B:",unb,B[b]);
	# override
	P["compiler.path"]="";
	P["build.f_cpu"]="80000000L"
	P["runtime.ide.version"]="10612"
	P["build.arch"]="ESP8266"
	P["build.path"]="/opt/esp8266-toolchain/Arduino-3.1.2/cores/esp8266"
	P["runtime.platform.path"]="/opt/esp8266-toolchain/Arduino-3.1.2";
	P["_id"]="generic";
	# remove build.opt
	P["build.opt.flags"]="";
	for k in ["recipe.cpp.o.pattern"]:
		v=resolve(P,P[k]);
		print(f"{k:s}=={v:s}");
		print("unresolved:",re.findall(r'{\S+}',v));
	#print(P["compiler.cpp.cmd"]);

if __name__ == "__main__":
	main();

#!/usr/bin/env python3

import re;
import sys;

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
		## print(key,"->",P[key]);
	return resolve(P,ret);

def subkeys(P,m,direct=True):
	ret=list();
	for k in P:
		if not k.startswith(m):
			continue;
		if not direct:
			ret.append(k);
			continue;
		if not "." in k[len(m)+1:]:
			ret.append(k);
	return ret;
	
def main():
	#coredir="/home/julien/delme/esp8266";
	coredir="/opt/esp8266-toolchain/Arduino-3.1.2";
	board="nodemcu";
	key="recipe.cpp.o.pattern"
	if len(sys.argv) > 1:
		coredir=sys.argv[1];
		board=sys.argv[2];
		key=sys.argv[3];
	platform=f"{coredir:s}/platform.txt"
	boards=f"{coredir:s}/boards.txt"
	P=read(platform);
	B=read(boards);
	direct=True;
	menus=dict();
	for b in B:
		if b.startswith("menu."):
			menus[b]=B[b];
		if b.startswith(board+"."):
			unb=".".join(b.split(".")[1:])
			if unb in P:
				if unb == "name":
					continue;
				print("clash at",unb);
				print("platforms:",P[unb]);
				print("using boards:",B[b]);
				#assert(False)
			P[unb]=B[b];
	P["compiler.path"]="";
	P["build.f_cpu"]="80000000L"
	P["runtime.ide.version"]="10612"
	P["build.arch"]="ESP8266"
	P["build.path"]=coredir+"/cores/esp8266"
	P["runtime.platform.path"]=coredir;
	P["_id"]="generic";
	# remove build.opt
	P["build.opt.flags"]="";
	
	v=resolve(P,P[key]);
	print(f"{v:s}");
	print("unresolved:",re.findall(r'{\S+}',v));
	
if __name__ == "__main__":
	main();

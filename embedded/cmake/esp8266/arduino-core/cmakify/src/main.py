#!/usr/bin/env python3

import re;
import sys;
import argparse;

arguments=None;
def parse_arguments():
	global arguments;
	parser = argparse.ArgumentParser();
	parser.add_argument('COREDIR',default="/opt/esp8266-toolchain/Arduino-3.1.2");
	parser.add_argument('-b', '--board', default="nodemcu",help="board from boards.txt");
	parser.add_argument('-k', '--key', default="recipe.cpp.o.pattern",help="key from platform.txt");
	# target_compile_definitions
	# target_compile_options
	# target_include_directories
	# target_link_libraries
	# target_link_options
	# target_sources
	parser.add_argument('-f', '--filter', default=None, help="{compile_definitions,compile_options,include_directories}");
	

	arguments=parser.parse_args();

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

def build():
	platform=f"{arguments.COREDIR:s}/platform.txt"
	boards=f"{arguments.COREDIR:s}/boards.txt"
	P=read(platform);
	B=read(boards);
	direct=True;
	menus=dict();
	for b in B:
		if b.startswith("menu."):
			menus[b]=B[b];
		if b.startswith(arguments.board+"."):
			unb=".".join(b.split(".")[1:])
			# note: prefer the board.txt over the platform.txt
			P[unb]=B[b];
	P["compiler.path"]="";
	P["build.f_cpu"]="80000000L"
	P["runtime.ide.version"]="10612"
	P["build.arch"]="ESP8266"
	P["build.path"]=arguments.COREDIR+"/cores/esp8266"
	P["runtime.platform.path"]=arguments.COREDIR;
	P["_id"]="generic";
	# remove build.opt
	P["build.opt.flags"]="";
	return P

def output(P):
	global arguments;
	if arguments.key in P:
		v=resolve(P,P[arguments.key]);
		print(f"{v:s}");
		print("unresolved:",re.findall(r'{\S+}',v));
	else:
		print("could not find key",arguments.key);

def main():
	global arguments;
	parse_arguments();
	P=build();
	output(P)
	
if __name__ == "__main__":
	main();

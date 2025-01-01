#!/usr/bin/env python3

import re;
import classify;

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

class Data:
	def __init__(self, arguments):
		self.arguments = arguments;
		platform=f"{arguments.COREDIR:s}/platform.txt"
		boards=f"{arguments.COREDIR:s}/boards.txt"
		self.P=read(platform);
		self.B=read(boards);
		for b in self.B:
			if b.startswith(arguments.board+"."):
				unb=".".join(b.split(".")[1:])
				# note: prefer the board.txt over the platform.txt
				self.P[unb]=self.B[b];
		self.P["compiler.path"]="";
		self.P["build.f_cpu"]="80000000L"
		self.P["runtime.ide.version"]="10612"
		self.P["build.arch"]="ESP8266"
		self.P["build.path"]=arguments.COREDIR+"/cores/esp8266"
		self.P["runtime.platform.path"]=arguments.COREDIR;
		self.P["_id"]="generic";
		# remove build.opt
		self.P["build.opt.flags"]="";

	def COREDIR(self):
		return self.arguments.COREDIR;

	def resolve(self,key):
		return resolve(self.P,self.P[key]);

	def classify(self,key):
		return classify.classify(self.resolve(key))

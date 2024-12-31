#!/usr/bin/env bash

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

def build(arguments):
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



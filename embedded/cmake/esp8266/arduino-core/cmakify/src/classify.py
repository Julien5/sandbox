#!/usr/bin/env python3

FLAGS="FLAGS";
INCLUDES="INCLUDES";
DEFINES="DEFINES";
TYPES={FLAGS,INCLUDES,DEFINES};

def classify_element(e):
	if e.startswith("-I"):
		return INCLUDES;
	for f in {"-D","-U"}:
		if e.startswith(f):
			return DEFINES;
	for f in {"-f","-M","-g","-O","-n","-m","-W","-std"}:
		if e.startswith(f):
			return FLAGS;
	return None;

def classify(raw):
	L=raw.split(" ");
	ret=dict();
	for t in TYPES:
		ret[t]=list();
	for l in L:
		if not l.strip():
			continue;
		e=l.replace("\"","");
		t=classify_element(e);
		if not t:
			#print("no type for:",e);
			continue;
		ret[t].append(e);
	return ret;
	

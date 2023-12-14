#!/usr/bin/env python3

import os;
import datetime;

def CreateChainDict():
	D={};
	for f in os.listdir("chain"):
		# $ cat chain/30.12.2022 
		# 3->4
		try:
			if os.path.isdir(os.path.join("chain/",f)):
				continue;
			filename=os.path.join("chain",f);
			date=datetime.datetime.strptime(f, "%d.%m.%Y");
			D[date]=open(filename,"r").read().split("\n")[0].split("->")[1];
		except ValueError as e:
			print("skip",f,"because",e,type(e));
			continue;
	return D;	

ChainDict=dict();
def getchain(s):
	global ChainDict;
	if not ChainDict:
		ChainDict=CreateChainDict();
	tourdate=s.begintime();
	ret="?";
	for chaindate in sorted(ChainDict):
		if tourdate.date()>chaindate.date():
			ret=ChainDict[chaindate];
	return ret;		

def chain_distances(T):
	ret=dict();
	for t in T:
		chain=getchain(t);
		if not chain in ret:
			ret[chain]=0;
		ret[chain]+=t.distance/1000;	
	for chain in ret:
		print(f"chain #{chain:s}: {ret[chain]:05.1f} km");
	return ret;

def main():
	pass;

if __name__ == '__main__':
	sys.exit(main())  

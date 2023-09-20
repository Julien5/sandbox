#!/usr/bin/env python3

import readgpx;
import os;
import sys;
import datetime;
import math;
import segments;
import output;

def readtours():
	test=False;
	#test=True;
	print("read files..");
	if not test:
		dir="/home/julien/tracks/";
		#dir="/home/julien/tracks/2022.11.25";
		# dir="test";
		if len(sys.argv)>1:
			dir=sys.argv[1];
		T=readgpx.tracksfromdir(dir);
		#T=T[0:20];
	else:
		T=readgpx.tracksfromdir("test");
	print("clean tracks..");
	assert(T);
	T=readgpx.clean(T);
	print("categorizing..");
	C=dict();
	for t in T:
		#t.stats();
		if not t.category() in C:
			C[t.category()]=list();
		C[t.category()].append(t);
	print("OK");
	return C;

def filter_tours(T,last_days=30):
	D=dict();
	for t in T:
		time=t.begintime().replace(tzinfo=None)
		D[time]=t;
	ret=list();
	now = datetime.datetime.now();
	for d in D:
		#print(d)
		#print(now)
		delta=now-d;
		if delta.days>last_days:
			continue;
		ret.append(D[d]);
	return ret;	

ChainDict=dict();
def getchain(track):
	global ChainDict;
	if not ChainDict:
		for f in os.listdir("chain"):
			try:
				d=datetime.datetime.strptime(f, "%d.%m.%Y");
				ChainDict[d]=open("chain/"+f,"r").read().split("\n")[0].split("->")[1];
			except Exception as e:
				#print("skip",f,"because",e);
				continue;
	ret="?";
	tourdate=track.begintime();
	for chaindate in sorted(ChainDict):
		if tourdate.date()>chaindate.date():
			ret=ChainDict[chaindate];
	return ret;		

def main():
	C=readtours();
	chaindistance=dict();
	for cat in ["cycling","running"]:
	#for cat in ["cycling"]:	
		S=dict();
		T=C[cat];
		Tf=filter_tours(T)
		if not T:
			print("ignore",cat);
			continue;
		for t in Tf:
			if not t.distance() in S:	
				S[t.distance()]=set();	
			S[t.distance()].add(t);
		print("# category",cat);
		#for d in sorted(S):	
		#	for t in S[d]:
		csv=str();
		for t in T:
			output.print_stats(t);
			chain=getchain(t);
			if not chain in chaindistance:
				chaindistance[chain]=0;
			chaindistance[chain]+=t.distance()/1000;	
		L=sum([t.distance() for t in Tf]);
		D=sum([t.duration().total_seconds() for t in Tf]);
		print(f"total-30 {cat:10s}: {L/1000:6.1f} km | {D/3600:4.1f}h");
		print("-"*55)
		for chain in chaindistance:
			print(f"chain #{chain:s}: {chaindistance[chain]:05.1f} km");
		csv=str();
		for t in T:
			csv+=output.print_csv(t);
			csv+=";";
			csv+=getchain(t)+";";
			csv+="\n"
		filename="/tmp/"+cat+".csv";
		print("write",filename);
		open(filename,'w').write(csv);
		#segments.main(T);
		print();

if __name__ == '__main__':
	sys.exit(main())  

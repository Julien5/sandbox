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

def readbook():
	d_cache=None;
	if os.path.exists("book.cache"):
		d_cache=datetime.datetime.fromtimestamp(os.path.getmtime("book.cache"));
	if not d_cache:
		print("from data");
		book=loadtours();
		writebook_tocache(book);
	else:
		print("from cache");
		book=loadtours();
	return book;

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

def main():
	C=readtours();
	for cat in ["cycling","running"]:
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
		for t in T:
			output.print_stats(t);
			readgpx.write(t,f"/tmp/{t.category():s}-{t.name():s}.gpx");
		L=sum([t.distance() for t in Tf]);
		D=sum([t.duration().total_seconds() for t in Tf]);
		print(f"total-30 {cat:10s}: {L/1000:6.1f} km | {D/3600:4.1f}h");
		print("-"*55)
		segments.main(T);
		print()
		
if __name__ == '__main__':
	sys.exit(main())  

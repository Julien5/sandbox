#!/usr/bin/env python3

import os;
import sys;
import datetime;

from . import plot;
from . import boxes;
from . import cells;
from . import coloredmap;
from . import segmentization;
from . import bbox;
from . import neighboor;
from . import gather;
from . import output;

def nboxes(B):
	return sum([len(b.boxes()) for b in B]);

def plot_trackarea(Cells,T,n,bb=None):
	areas=[c.area() for c in Cells if n in c.color()];
	colors=[c.color() for c in Cells if n in c.color()];
	tracks=T;
	if not bb:
		bb=bbox.cells([c for c in Cells if n in c.color()]);
	plot.plot_areas(areas,colors,tracks,bb,"/tmp/trackarea-{}.gnuplot".format(n));

def celllist(Cells,I,hitlist):
	sublist=list();
	for s in hitlist:
		for i in I:
			if s in Cells[i].area():
				if not sublist or i != sublist[-1]:
						sublist.append(i);
	return sublist;					
	
def getCellList(Cells,T,index):
	assert(type(index)==type(0));
	friends=set();
	L=list();
	I=neighboor.cells_with_color(Cells,index);
	assert(I);
	G=T[index].geometry()
	for k in range(len(G)-1):
		u=G[k];
		v=G[k+1];
		sublist=celllist(Cells,I,boxes.hitlist(u,v));
		for cellIndex in sublist:
			assert(cellIndex != None);
			if not L or {cellIndex} != L[-1]:
				L.append({cellIndex});
	assert(L);
	# Because the set I includes surroundings, we have:
	# len(I) <= len(L)
	# but because L go hit the same cell multiple times, we have
	# len(I) => len(L)
	# => we cannot assert anything about there lengths.
	return L;		

def processSingleTrack(Cells,T,index):
	L=getCellList(Cells,T,index);
	# print("index:", index," cells:",L);
	acc=gather.Accumulator();
	assert(L);
	assert(type(L[0]) == type(set()));
	gather.walk(Cells,L,acc);
	R=acc.result();
	# L = [{1},{2},{3}] (cell indexes)
	# R[color] is a set. Each element is a set of index.
	# example:
	# R[(1)] = {{1,3,4},{6}}
	assert(acc.check(Cells,L));
	output.process(Cells,T,cells.SegmentsDictionary(acc.result()),index);

def computeCells(T):
	B=dict();
	for k in range(len(T)):
		B[k]=boxes.boxes(T[k]);
	Map=coloredmap.indexes(coloredmap.colors(B));
	Cells=list();
	assert(Map);
	for color in Map:
		S=segmentization.subgroups(Map[color]);
		for s in S:
			# ignore cells with one track only.	
			#if len(s) == 1:
			#	continue;
			Cells.append(cells.Cell(s,set(color)));
	assert(Cells);		
	return Cells;		

def processtracks(Cells,T):
	#A=[c.area() for c in Cells];
	#C=[len(c.color()) for c in Cells];
	#plot.plot_areas(A,C,T,bbox.cells(Cells),"/tmp/map.gnuplot");
	for color in [len(T)-1]:#range(len(T)):
		processSingleTrack(Cells,T,color);	

class Book:
	def __init__(self,Cells,C):
		self._cells=Cells;
		self._C=C;

	def cells(self,cat):
		if not cat in self._cells:
			return None;
		return self._cells[cat];

	def tours(self,cat):
		if not cat in self._C:
			return None;
		return self._C[cat];

import pickle
def writebook_tocache(b):
	with open('book.cache', 'wb') as f:
		pickle.dump(b, f);

def loadbook_fromcache():
	if not os.path.exists("book.cache"):
		return None;
	with open('book.cache', 'rb') as f:
		return pickle.load(f);
	assert(0);

def loadbook_fromdata():
	test=False;
	#test=True;
	print("read files..");
	if not test:
		dir="/home/julien/tracks/";
		# dir="/home/julien/tracks/2022.11.25";
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
	Cells=dict();
	for cat in ["cycling","running"]:
		Cells[cat]=computeCells(C[cat]);
	return Book(Cells,C);

def readbook():
	d_cache=None;
	if os.path.exists("book.cache"):
		d_cache=datetime.datetime.fromtimestamp(os.path.getmtime("book.cache"));
	if not d_cache:
		print("from data");
		book=loadbook_fromdata();
		writebook_tocache(book);
	else:
		print("from cache");
		book=loadbook_fromcache();
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

def main(T):
	cells=computeCells(T);
	processtracks(cells,T);
		


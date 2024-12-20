#!/usr/bin/env python3

import sorttracks;
import os;
import statistics;
import utils;
import chain;

from glob import glob

def create_statistics(dirs,force=False):
	for n in range(len(dirs)):
		dirname=dirs[n];
		percent=100*n/len(dirs);
		alltxt=os.path.join(dirname,"gpx","GAP00-all.txt")
		if force or not os.path.exists(alltxt):
			sorttracks.create_statistics(dirname);
			print(f"{dirname:50s} [{percent:04.1f}%]",end="");
			print("\n",end="",flush="True");

def statsfilesH(dirname):
	return glob(dirname+"/**/GAP*-*-*.txt", recursive=True);

def statsfilesD(dirname):
	return glob(dirname+"/**/*-*.txt", recursive=True);

def readallstatsD(directory):
	D={};
	for filename in statsfilesD(directory):
		stats=statistics.readstats(filename);
		D[stats.startpoint.time]=stats;
	for time in sorted(D.keys()):
		s=D[time];
		statistics.print_statistics(s);

def gather_statistics(dirs):
	D={};
	for dirname in dirs:
		for filename in statsfilesH(dirname):
			stats=statistics.readstats(filename);
			D[stats.startpoint.time]=stats;
			#print(stats.startpoint.time,filename,stats.typename,stats.distance);

	last_month=None;		
	acc={};
	T=sorted(D.keys())
	Tcycling=list();
	for k in range(len(T)):
		time=T[k];
		month=time.strftime("%m.%Y");
		month_changed = month != last_month;

		if month_changed:
			print("-"*10)
			for key in sorted(acc.keys()):
				statistics.print_statistics_friendly(acc[key]);
			acc={};	
			print();
			print(month);

		s=D[time];
		if s.typename == "moving" and s.distance>1000:
			#statistics.print_statistics_friendly(s);
			statistics.print_statistics(s);
			if statistics.category(s) == "cycling":
				Tcycling.append(s);
			key=statistics.category(s);
			if key not in acc:
				acc[key]=s;
			else:
				acc[key].accumulate(s);

		last_month=month;
	return D,acc,Tcycling;	

def main():
	dirs=glob("/home/julien/projects/tracks/*/", recursive=False);
	#dirs=glob("test/H/*/", recursive=False);
	create_statistics(dirs,force=False);
	all,acc,Tcycling=gather_statistics(dirs);
	if acc:
		print("-"*10)
		for key in sorted(acc.keys()):
			statistics.print_statistics_friendly(acc[key]);

	chain.chain_distances(Tcycling);
	chain.distance_current_chain(Tcycling);

if __name__ == "__main__":
	main();

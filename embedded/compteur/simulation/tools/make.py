#!/usr/bin/env python3

import sys;
import os;

def readlines(filename):
	f=open(filename,'r');
	return f.readlines();

def writelines(blocks,filename):
	f=open(filename,'a');
	for b in blocks:
		f.write(" ".join([str(x) for x in b])+"\n");
		
def numbers(line):
	ret=list();
	C=line.split(" ");
	for c in C:
		if c.isnumeric():
			n=int(c);
			if n > 0:
				ret.append(n);
	return ret;		

adcvalues_cache=dict();
def adcvalues(filename):
	global adcvalues_cache;
	
	if filename in adcvalues_cache:
		return adcvalues_cache[filename];
	
	lines=readlines(filename);
	ret=list();
	for line in lines:
		if "time" in line:
			continue;
		l=numbers(line);
		if l:
			ret.append(l);
	adcvalues_cache[filename]=ret;
	return ret;

def number_of_blocks_per_second():
	# blocks are recorded every 200ms 
	return int(1000/200);

def red_mark_width():
	return 1.0/20;

def number_of_rotation_per_kWh():
	return 100;

def getwhiteblocks(duration_s):
	values=adcvalues("simulation/data/real_white.txt")
	ret=list();
	count=round(duration_s*number_of_blocks_per_second());
	assert(count>0);
	for c in range(count):
		ret.append(values[c % len(values)]);
		
	return ret;

def getredblocks(duration_s):
	values=adcvalues("simulation/data/real_red_1.txt")
	ret=list();
	count=round(duration_s*number_of_blocks_per_second());
	assert(count>0);
	for c in range(count):
		ret.append(values[c % len(values)]);
	return ret;

# assuming 100 us per adc shot (10kSPS)
# and R=100U/kWh and W=1/20 mark width

def main():
	args=sys.argv;
	
	power=1000; # 1 kWk
	if len(args)>1:
		power=int(sys.argv[1]);

	duration_s=float(1000/power)*float(100/number_of_rotation_per_kWh())*36;
	duration_red=red_mark_width()*duration_s;
	duration_white=duration_s-duration_red;
	
	if os.path.exists("output"):
		os.remove("output");
		
	WB=getwhiteblocks(duration_white);		
	writelines(WB,'output');
	RB=getredblocks(duration_red);
	writelines(RB,'output');
	
if __name__ == '__main__': 
	main();

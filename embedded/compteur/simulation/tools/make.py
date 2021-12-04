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

g_offset=dict();
def offset(filename):
	global g_offset;
	if not filename in g_offset:
		g_offset[filename]=0;
	g_offset[filename]=g_offset[filename]+1;
	return g_offset[filename];

def getwhiteblocks(duration_s):
	filename="simulation/data/real_white.txt";
	values=adcvalues(filename);
	ret=list();
	count=round(duration_s*number_of_blocks_per_second());
	assert(count>0);
	for c in range(count):
		ret.append(values[offset(filename) % len(values)]);
	return ret;

def getredblocks(duration_s):
	filename="simulation/data/real_red_1.txt";
	values=adcvalues(filename)
	ret=list();
	count=round(duration_s*number_of_blocks_per_second());
	assert(count>0);
	for c in range(count):
		ret.append(values[offset(filename) % len(values)]);
	return ret;

# assuming 100 us per adc shot (10kSPS)
# and R=100U/kWh and W=1/20 mark width

def readarg(name,default):
	A=dict();
	for arg in sys.argv:
		if "=" in arg:
			key=arg.split("=")[0];
			if key == name:
				return int(arg.split("=")[1]);
	return default;		

def main():
	args=sys.argv;
	
	power=readarg("power",1000); # 1 kWk

	duration_s=float(1000/power)*float(100/number_of_rotation_per_kWh())*36;
	duration_red=red_mark_width()*duration_s;
	duration_white=duration_s-duration_red;
	
	if os.path.exists("output"):
		os.remove("output");

	repetitions=readarg("repetitions",1); # 1 kWk
	for k in range(repetitions):
		WB=getwhiteblocks(duration_white);		
		writelines(WB,'output');
		RB=getredblocks(duration_red);
		writelines(RB,'output');
	
if __name__ == '__main__': 
	main();

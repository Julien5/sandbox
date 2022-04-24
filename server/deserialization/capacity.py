#!/usr/bin/env python3

from readints import *;

def read_u8(bytes,pos):
	end=pos+1
	return end,int.from_bytes(bytes[pos:end], byteorder='little', signed=False)    

class Capacity:
	def __init__(self,bytes,time):
		pos=0
		self.esp = read_ui16(bytes,pos);pos+=2;
		self.arduino = read_ui16(bytes,pos); pos+=2;
		self.time=time;
		
	def __str__(self):
		return "%s: arduino:%d esp:%d" % (self.time.strftime("%d.%m %H:%M"),self.arduino, self.esp)

if __name__ == "__main__":
	main();

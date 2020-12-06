#!/usr/bin/env python3

def read_u8(bytes,pos):
	end=pos+1
	return end,int.from_bytes(bytes[pos:end], byteorder='little', signed=False)    

class Status:
	def __init__(self,bytes):
		pos=0
		pos,self.calibrated = read_u8(bytes,pos);
		pos,self.line = read_u8(bytes,pos);
		pos,self.m = read_u8(bytes,pos);
		pos,self.M = read_u8(bytes,pos);
		pos,self.T0 = read_u8(bytes,pos);
		pos,self.v1 = read_u8(bytes,pos);
		pos,self.v2 = read_u8(bytes,pos);
		pos,self.v = read_u8(bytes,pos);
		pos,self.TH = read_u8(bytes,pos);
		pos,self.TL = read_u8(bytes,pos);
		
	def __str__(self):
		return "%02d %02d %02d %02d" % (self.m,self.TL,self.TH,self.M)

def main():
	b=open("/tmp/status.packed.hex",'rb').read().decode("utf-8");
	for h in b.split("\n"):
		b=bytes.fromhex(h);
		s=Status(b)
		for k in range(0,64):
			print(s)

if __name__ == "__main__":
	main();

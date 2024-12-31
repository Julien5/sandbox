#!/usr/bin/env python3

import argparse;
import resolve
import build
import classify;
import cmake;

arguments=None;
def parse_arguments():
	global arguments;
	parser = argparse.ArgumentParser();
	parser.add_argument('COREDIR',default="/opt/esp8266-toolchain/Arduino-3.1.2");
	parser.add_argument('-b', '--board', default="nodemcu",help="board from boards.txt");
	parser.add_argument('-k', '--key', default=None,help="key from platform.txt");
	# target_compile_definitions
	# target_compile_options
	# target_include_directories
	# target_link_libraries
	# target_link_options
	# target_sources
	parser.add_argument('-f', '--filter', default=None, help="{CFLAGS,CXXFLAGS}");
	arguments=parser.parse_args();

def output(P):
	global arguments;
	r=resolve.resolve(P,P[arguments.key]);
	C=classify.classify(r);		
	if arguments.filter:
		F=arguments.filter.split(",");
		for k in F:
			if not k in classify.TYPES:
				print("unknown",k);
				continue;
			if k in C:
				print("\n".join(C[k]));
	elif arguments.key:
		print(r);

def main():
	global arguments;
	parse_arguments();
	P=build.build(arguments);
	output(P)
	print(cmake.toolchain(P))
	
if __name__ == "__main__":
	main();

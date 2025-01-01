#!/usr/bin/env python3

import argparse;
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
	parser.add_argument('-t', '--toolchain', action='store_true', help="generate toolchain");
	arguments=parser.parse_args();

def output(data):
	global arguments;
	r=None
	C=None;
	if arguments.key:
		r=data.resolve(arguments.key);
		C=classify.classify(r);		
	if arguments.filter:
		F=arguments.filter.split(",");
		for k in F:
			if not k in classify.TYPES:
				print("unknown",k);
				continue;
			if k in C:
				print("\n".join(C[k]));
	elif arguments.toolchain:
		print(cmake.toolchain(data))
	elif arguments.key:
		print(r);

def main():
	global arguments;
	parse_arguments();
	data=build.Data(arguments);
	output(data)
	
if __name__ == "__main__":
	main();

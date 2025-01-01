#!/usr/bin/env python3

import classify;
import os;
import glob

buffer=list();

def format(L,coredir):
	ret=L[0];
	if len(L)>1:
		ret="\""+" ".join(L)+"\"";
	ret=ret.replace(coredir,"${COREDIR}");
	return ret;

class CMakeBuffer:
	def __init__(self):
		self.buffer=list();
		
	def set(self,varname,value):
		self.buffer.append(f"set({varname:s} {value:s})");

	def newline(self):
		self.buffer.append("");

	#project(common)
	def project(self,name):
		self.buffer.append(f"project({name:s})");
		
	#add_library(${PROJECT_NAME} STATIC)
	def add_library(self):
		self.buffer.append("add_library(${PROJECT_NAME} STATIC)");

	def target_sources(self,L):
		self.buffer.append("target_sources(${PROJECT_NAME} PRIVATE");
		for l in L:
			self.buffer.append(f"    {l:s}");
		self.buffer.append("    )");

	def target_include_directories(self,public,private):
		self.buffer.append("target_include_directories(${PROJECT_NAME}");
		self.buffer.append(f"    PUBLIC");
		for l in public:
			self.buffer.append(f"    {l:s}");
		self.buffer.append(f"    PRIVATE");
		for l in private:
			self.buffer.append(f"    {l:s}");
		self.buffer.append("    )");

	def data(self):
		return "\n".join(self.buffer);

def toolchain(data):
	cmake=CMakeBuffer();
	cmake.set("CMAKE_SYSTEM_NAME","Generic");
	cmake.newline();
	cmake.set("CMAKE_C_COMPILER",data.resolve("compiler.c.cmd"));
	cmake.set("CMAKE_CXX_COMPILER",data.resolve("compiler.cpp.cmd"))
	cmake.set("CMAKE_ASM_COMPILER",data.resolve("compiler.as.cmd"))
	cmake.newline();
	cmake.set("ARDUINOCOREDIR",data.COREDIR());
	cmake.newline();
	C=classify.classify(data.resolve("recipe.c.o.pattern"))
	cmake.set("CMAKE_C_FLAGS",format(C[classify.FLAGS],data.COREDIR()));
	C=classify.classify(data.resolve("recipe.cpp.o.pattern"))
	cmake.set("CMAKE_CXX_FLAGS",format(C[classify.FLAGS],data.COREDIR()));
	cmake.newline();
	C=classify.classify(data.resolve("recipe.c.combine.pattern"))
	cmake.set("CMAKE_EXE_LINKER_FLAGS",format(C[classify.FLAGS],data.COREDIR()))
	return cmake.data();

def library(data):
	libdir=data.arguments.library
	cmake=CMakeBuffer();
	cmake.project(os.path.basename(libdir));
	cmake.add_library();
	
	files = glob.glob(f"{libdir:s}/**/*.cpp", recursive=True)
	L=[format([f],data.COREDIR()) for f in files];
	cmake.target_sources(L);


	public=[format([f"{libdir:s}/src"],data.COREDIR())];
	private=[format([f"{libdir:s}/src/common"],data.COREDIR())];
	cmake.target_include_directories(public,private);
	
	return cmake.data();

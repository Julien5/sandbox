#!/usr/bin/env python3

import classify;
import os;
import glob

buffer=list();

def format(L,coredir):
	ret=L[0];
	if len(L)>1:
		ret="\""+" ".join(L)+"\"";
	ret=ret.replace(coredir,"${ARDUINOCOREDIR}");
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
		if public:
			self.buffer.append(f"    PUBLIC");
			for l in public:
				self.buffer.append(f"    {l:s}");
		if private:
			self.buffer.append(f"    PRIVATE");
			for l in private:
				self.buffer.append(f"    {l:s}");
		self.buffer.append("    )");

	def target_link_libraries(self,public,private):
		self.buffer.append("target_link_libraries(${PROJECT_NAME}");
		if public:
			self.buffer.append(f"    PUBLIC");
			for l in public:
				self.buffer.append(f"    {l:s}");
		if private:		
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
	C=data.classify("recipe.c.o.pattern")
	A=data.classify("menu.mmu.3232.build.mmuflags");
	C0=list();
	C0.extend(C[classify.FLAGS]);
	C0.extend(C[classify.DEFINES]);
	cmake.set("CMAKE_C_FLAGS",format(C0,data.COREDIR()));
	C=data.classify("recipe.cpp.o.pattern")
	C0=list();
	C0.extend(C[classify.FLAGS]);
	C0.extend(A[classify.DEFINES]);
	cmake.set("CMAKE_CXX_FLAGS",format(C0,data.COREDIR()));
	cmake.newline();
	#C=classify.classify(data.resolve("recipe.c.combine.pattern"))
	#cmake.set("CMAKE_EXE_LINKER_FLAGS",format(C[classify.FLAGS],data.COREDIR()))
	return cmake.data();

def library(data):
	libdir=data.arguments.library
	cmake=CMakeBuffer();
	libname=os.path.basename(libdir).lower();
	LIBNAME=libname.upper();
	cmake.project(os.path.basename(libdir));
	cmake.add_library();
	cmake.set("LIBDIR",format([libdir],data.COREDIR()));
	libformat=lambda f:format([f],data.COREDIR()).replace(libdir,"${LIBDIR}");
	files = glob.glob(f"{libdir:s}/**/*.cpp", recursive=True)
	L=[libformat(f) for f in files];
	cmake.target_sources(sorted(L));

	public=list();
	if data.arguments.add_include_directories:
		public.extend([libformat(d) for d in data.arguments.add_include_directories]);
	for f in [f"{libdir:s}",f"{libdir:s}/src"]:
		if os.path.isdir(f):
			public.append(libformat(f));
	private=list();
	for f in [f"{libdir:s}/src",f"{libdir:s}/src/common"]:
		if os.path.isdir(f):
			private.append(libformat(f));
	cmake.newline()
	cmake.target_include_directories(public,private);
	cmake.newline()
	public=list();
	private=list();
	if data.arguments.add_link_libraries:
		public.extend([libformat(d) for d in data.arguments.add_link_libraries]);
	cmake.target_link_libraries(public,private);
	
	cmake.newline()
	return cmake.data();

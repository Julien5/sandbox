#!/usr/bin/env python3

import classify;

buffer=list();

def format(L,coredir):
	ret="\""+" ".join(L)+"\"";
	ret=ret.replace(coredir,"${COREDIR}");
	return ret;

def set(varname,value):
	buffer.append(f"set({varname:s} {value:s})");

def toolchain(data):
	global buffer;
	buffer=list()
	set("CMAKE_SYSTEM_NAME","Generic");
	buffer.append("");
	set("CMAKE_C_COMPILER",data.resolve("compiler.c.cmd"));
	set("CMAKE_CXX_COMPILER",data.resolve("compiler.cpp.cmd"))
	set("CMAKE_ASM_COMPILER",data.resolve("compiler.as.cmd"))
	buffer.append("");
	set("ARDUINOCOREDIR",data.COREDIR());
	buffer.append("");
	C=classify.classify(data.resolve("recipe.c.o.pattern"))
	set("CMAKE_C_FLAGS",format(C[classify.FLAGS],data.COREDIR()));
	C=classify.classify(data.resolve("recipe.cpp.o.pattern"))
	set("CMAKE_CXX_FLAGS",format(C[classify.FLAGS],data.COREDIR()));
	buffer.append("");
	C=classify.classify(data.resolve("recipe.c.combine.pattern"))
	set("CMAKE_EXE_LINKER_FLAGS",format(C[classify.FLAGS],data.COREDIR()))
	return "\n".join(buffer);

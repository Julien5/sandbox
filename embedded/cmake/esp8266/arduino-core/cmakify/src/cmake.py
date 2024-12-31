#!/usr/bin/env bash

import classify;
import resolve;

buffer=list();

def set(varname,value):
	buffer.append(f"set({varname:s} {value:s})");

def toolchain(P):
	global buffer;
	buffer=list()
	set("CMAKE_SYSTEM_NAME","Generic");
	buffer.append("");
	set("CMAKE_C_COMPILER",resolve.resolve(P,P["compiler.c.cmd"]))
	set("CMAKE_CXX_COMPILER",resolve.resolve(P,P["compiler.cpp.cmd"]))
	set("CMAKE_ASM_COMPILER",resolve.resolve(P,P["compiler.as.cmd"]))
	buffer.append("");
	C=classify.classify(resolve.resolve(P,P["recipe.c.o.pattern"]))
	set("CMAKE_C_FLAGS","\""+" ".join(C[classify.FLAGS])+"\"")
	C=classify.classify(resolve.resolve(P,P["recipe.cpp.o.pattern"]))
	set("CMAKE_CXX_FLAGS","\""+" ".join(C[classify.FLAGS])+"\"")
	buffer.append("");
	C=classify.classify(resolve.resolve(P,P["recipe.c.combine.pattern"]))
	set("CMAKE_EXE_LINKER_FLAGS","\""+" ".join(C[classify.FLAGS])+"\"")
	return "\n".join(buffer);

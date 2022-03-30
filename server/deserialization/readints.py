#!/usr/bin/env python3

def read_ui64(bytes,pos):
    return int.from_bytes(bytes[pos:pos+8], byteorder='little', signed=True)

def read_ui32(bytes,pos):
    return int.from_bytes(bytes[pos:pos+4], byteorder='little', signed=True)

def read_ui16(bytes,pos):
    return int.from_bytes(bytes[pos:pos+2], byteorder='little', signed=True)

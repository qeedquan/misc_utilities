#!/usr/bin/env python

from ctypes import *

class Value(Structure):
    _fields_ = [
        ("x", c_int),
        ("y", c_int),
        ("z", c_int),
        ("fx", c_float),
        ("fy", c_float),
        ("fz", c_float),
        ("s1", c_char * 64),
        ("s2", c_char * 32)
    ]

def main():
    value = (Value*10)()

    api = CDLL("api.so")
    api.fill(pointer(value), len(value))

    for v in value:
        print(v.x, v.y, v.z, v.fx, v.fy, v.fz, v.s1, v.s2)

main()

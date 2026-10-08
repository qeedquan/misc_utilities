#!/usr/bin/env python

from ctypes import *
import struct
import copy
import sys

class Packed(Structure):
    _layout_ = "ms"
    _pack_ = 1
    _fields_ = [
        ("a", (c_char * 3)),
        ("b", c_short),
        ("c", c_int),
    ]

class Point(Structure):
    _fields_ = [
        ("x", c_float),
        ("y", c_float),
        ("z", c_float),
    ]

class Value(Structure):
    _fields_ = [
        ("x", c_int),
        ("y", c_int),
        ("z", c_int),
        ("fx", c_float),
        ("fy", c_float),
        ("fz", c_float),
        ("s1", c_char * 64),
        ("s2", c_char * 32),
        ("pt", Point * 10),
        ("fp", POINTER(c_float * 80)),
        ("nfp", c_int),
    ]

    def set_xyz(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __repr__(self):
        return "%d %d %d | %f %f %f | %d" % (self.x, self.y, self.z, self.fx, self.fy, self.fz, self.nfp)


def main():
    value = (Value * 10)()

    api = CDLL("api.so")
    api.fill(pointer(value), len(value))

    for v in value:
        print(v.x, v.y, v.z, v.fx, v.fy, v.fz, v.s1, v.s2)
        for i in range(10):
            print(v.pt[i].x, v.pt[i].y, v.pt[i].z, end=" ")
        print()
        print()

    value[0].set_xyz(-1, -2, -3)
    print(value[0])

    # deep copies can only handle value types
    # if the structure has a pointer type, it will fail
    #value2 = copy.deepcopy(value)
    value2 = (Value * 10)()
    value2[0].set_xyz(4, 5, 6)
    print(value[0])
    print(value2[0])
    api.show(pointer(value), len(value))

    p = Packed()
    p.a = b"\x01\x02\x03"
    p.b = 30
    p.c = 50
    api.packed(pointer(p))

    if sys.platform.startswith("win32"):
        libc = CDLL("msvcrt")
    else:
        libc = CDLL(None)

    libc.malloc.argtypes = [c_size_t]
    libc.malloc.restype = c_void_p

    num_elements = 10
    byte_size = num_elements * sizeof(Point)
    raw_pointer = libc.malloc(byte_size)
    array_pointer = cast(raw_pointer, POINTER(Point * num_elements))
    
    # Write values to the allocated memory block
    for i in range(num_elements):
        array_pointer.contents[i] = Point(i, i+1, i+2)
    
    print()
    api.points(array_pointer, num_elements)

main()

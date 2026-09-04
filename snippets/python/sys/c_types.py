#!/usr/bin/env python

from ctypes import *

def main():
    print("bool", sizeof(c_bool()))
    print("char", sizeof(c_char()))
    print("wchar", sizeof(c_wchar()))
    print("byte", sizeof(c_byte()))
    print("ubyte", sizeof(c_ubyte()))
    print("short", sizeof(c_short()))
    print("ushort", sizeof(c_ushort()))
    print("int", sizeof(c_int()))
    print("uint", sizeof(c_uint()))
    print("long", sizeof(c_long()))
    print("ulong", sizeof(c_ulong()))
    print("longlong", sizeof(c_longlong()))
    print("ulonglong", sizeof(c_ulonglong()))
    print("size_t", sizeof(c_size_t()))
    print("ssize_t", sizeof(c_ssize_t()))
    print("float", sizeof(c_float()))
    print("double", sizeof(c_double()))
    print("longdouble", sizeof(c_longdouble()))
    print("char_p", sizeof(c_char_p()))
    print("wchar_p", sizeof(c_wchar_p()))
    print("void_p", sizeof(c_void_p()))

main()

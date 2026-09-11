#!/usr/bin/env python

import sys

def main():
    print("max size:", sys.maxsize)
    print("flags:", sys.flags)
    print("float_info:", sys.float_info)
    print("allocated blocks:", sys.getallocatedblocks())
    print("recursion limit:", sys.getrecursionlimit())
    print("platform:", sys.platform)
    print("current frames:", sys._current_frames())
    print("thread info:", sys.thread_info)

main()

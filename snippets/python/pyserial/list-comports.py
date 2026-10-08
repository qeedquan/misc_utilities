#!/usr/bin/env python

from serial.tools.list_ports import *

def main():
    for c in comports():
        print(c)
        for v in c:
            print(v)
        print()

main()
